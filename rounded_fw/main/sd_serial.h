/**
 * @file sd_serial.h
 *
 * Read the ride logs over the USB-C serial port, without taking the SD card out.
 * The PC side is tools/sdcard.py. Commands, one per line:
 *
 *   LS          -> #LS <name> <size>  ...  #END
 *   GET <name>  -> #FILE <name> <size>, #D <base64, 96 bytes per line> ..., #END <crc32 hex>
 *   DEL <name>  -> #OK
 *
 * Any failure answers #ERR <reason>. Lines without a leading # are the usual log
 * output and are ignored by the PC side. Works only while not recording.
 */
#ifndef SD_SERIAL_H
#define SD_SERIAL_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Install the USB serial driver (the log keeps working through it) and start the command task */
esp_err_t sd_serial_start(void);

#ifdef __cplusplus
}
#endif

#endif /* SD_SERIAL_H */
