/**
 * @file i2c_lock.h
 *
 * One lock for the shared I2C bus (GPIO15/14), taken by the IMU, the PMU and the GPS.
 *
 * The LC76G GPS needs it: Quectel requires that no other device is accessed during
 * a GPS transaction (length request -> data read), or the module can stop
 * answering at 0x54 until a long power-off. The driver only serialises single
 * transfers, not that sequence. (The touch controller, driven by the BSP, can't
 * take this lock; it only talks while the screen is touched.)
 */
#ifndef I2C_LOCK_H
#define I2C_LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

void i2c_lock(void);
void i2c_unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_LOCK_H */
