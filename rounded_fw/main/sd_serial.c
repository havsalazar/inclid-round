/**
 * @file sd_serial.c
 *
 * See sd_serial.h for the protocol. The file data goes as base64 lines with a
 * CRC-32 at the end: the log output shares the port, and whole lines from it can
 * appear between ours, but never inside one (each line is a single driver write).
 */
#include "sd_serial.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "esp_log.h"

#include "bsp/esp-bsp.h"            /* BSP_SD_MOUNT_POINT */
#include "logger.h"

#define CMD_MAX_LEN     64
#define CHUNK_BYTES     96          /* -> 128 base64 characters per line */
#define TX_TIMEOUT_MS   500         /* drop the output if no one is reading */

static const char * TAG = "sd_serial";

/*********************
 *  OUTPUT
 *********************/
static void out(const char * s, size_t len)
{
    usb_serial_jtag_write_bytes(s, len, pdMS_TO_TICKS(TX_TIMEOUT_MS));
}

static void outf(const char * fmt, ...) __attribute__((format(printf, 1, 2)));
static void outf(const char * fmt, ...)
{
    char line[160];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if(n < 0) return;
    if(n >= (int)sizeof(line)) n = sizeof(line) - 1;
    out(line, n);
}

/* The same as Python's zlib.crc32 */
static uint32_t crc32_update(uint32_t crc, const uint8_t * p, size_t n)
{
    crc = ~crc;
    while(n--) {
        crc ^= *p++;
        for(int k = 0; k < 8; k++) crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

static size_t base64(const uint8_t * in, size_t n, char * out_buf)
{
    static const char T[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t o = 0;
    for(size_t i = 0; i < n; i += 3) {
        uint32_t v = in[i] << 16;
        if(i + 1 < n) v |= in[i + 1] << 8;
        if(i + 2 < n) v |= in[i + 2];
        out_buf[o++] = T[(v >> 18) & 63];
        out_buf[o++] = T[(v >> 12) & 63];
        out_buf[o++] = i + 1 < n ? T[(v >> 6) & 63] : '=';
        out_buf[o++] = i + 2 < n ? T[v & 63] : '=';
    }
    return o;
}

/*********************
 *  COMMANDS
 *********************/
/* Plain names in the card's root only: no paths */
static bool name_ok(const char * name)
{
    if(name[0] == '\0' || strlen(name) > 12) return false;
    for(const char * p = name; *p; p++) {
        if(!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') ||
             *p == '.' || *p == '_' || *p == '-')) return false;
    }
    return true;
}

static void cmd_ls(void)
{
    DIR * dir = opendir(BSP_SD_MOUNT_POINT);
    if(dir == NULL) {
        outf("#ERR can't read the card\n");
        return;
    }
    struct dirent * e;
    char path[48];
    struct stat st;
    while((e = readdir(dir)) != NULL) {
        if(e->d_type == DT_DIR) continue;
        snprintf(path, sizeof(path), BSP_SD_MOUNT_POINT "/%.13s", e->d_name);
        long size = stat(path, &st) == 0 ? (long)st.st_size : -1;
        outf("#LS %s %ld\n", e->d_name, size);
    }
    closedir(dir);
    outf("#END\n");
}

static void cmd_get(const char * name)
{
    char path[48];
    snprintf(path, sizeof(path), BSP_SD_MOUNT_POINT "/%s", name);
    FILE * f = fopen(path, "rb");
    if(f == NULL) {
        outf("#ERR no such file: %s\n", name);
        return;
    }
    struct stat st;
    long size = stat(path, &st) == 0 ? (long)st.st_size : -1;
    outf("#FILE %s %ld\n", name, size);

    static uint8_t buf[CHUNK_BYTES];
    static char line[4 + (CHUNK_BYTES / 3) * 4 + 2];
    uint32_t crc = 0;
    size_t n;
    memcpy(line, "#D ", 3);
    while((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        crc = crc32_update(crc, buf, n);
        size_t len = 3 + base64(buf, n, line + 3);
        line[len++] = '\n';
        out(line, len);
    }
    fclose(f);
    outf("#END %08lx\n", (unsigned long)crc);
}

static void cmd_del(const char * name)
{
    char path[48];
    snprintf(path, sizeof(path), BSP_SD_MOUNT_POINT "/%s", name);
    if(remove(path) == 0) outf("#OK\n");
    else outf("#ERR can't delete %s\n", name);
}

static void handle(char * cmd)
{
    char * arg = strchr(cmd, ' ');
    if(arg) *arg++ = '\0';
    else arg = "";

    bool ls = strcmp(cmd, "LS") == 0, get = strcmp(cmd, "GET") == 0, del = strcmp(cmd, "DEL") == 0;
    if(!ls && !get && !del) {
        outf("#ERR unknown command (LS, GET <name>, DEL <name>)\n");
        return;
    }
    if(!ls && !name_ok(arg)) {
        outf("#ERR bad file name\n");
        return;
    }

    const char * why;
    if(!logger_card_acquire(&why)) {
        outf("#ERR %s\n", why);
        return;
    }
    if(ls) cmd_ls();
    else if(get) cmd_get(arg);
    else cmd_del(arg);
    logger_card_release();
}

static void sd_serial_task(void * arg)
{
    (void)arg;
    char cmd[CMD_MAX_LEN];
    size_t len = 0;
    uint8_t c;

    while(1) {
        if(usb_serial_jtag_read_bytes(&c, 1, portMAX_DELAY) != 1) continue;
        if(c == '\r') continue;
        if(c != '\n') {
            if(len + 1 < sizeof(cmd)) cmd[len++] = (char)c;
            continue;
        }
        cmd[len] = '\0';
        len = 0;
        if(cmd[0] == '\0') continue;
        ESP_LOGI(TAG, "%s", cmd);
        handle(cmd);
    }
}

esp_err_t sd_serial_start(void)
{
    usb_serial_jtag_driver_config_t cfg = {
        .tx_buffer_size = 4096,
        .rx_buffer_size = 256,
    };
    esp_err_t err = usb_serial_jtag_driver_install(&cfg);
    if(err != ESP_OK) return err;
    usb_serial_jtag_vfs_use_driver();               /* the log now shares the driver */

    return xTaskCreatePinnedToCore(sd_serial_task, "sd_serial", 4096, NULL, 2, NULL, 0) == pdPASS
           ? ESP_OK : ESP_ERR_NO_MEM;
}
