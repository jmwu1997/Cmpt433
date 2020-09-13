
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x1C
#define WHO_AM_I 0x2A
#define ZERO 0x00
#define OUT_X_MSB 0x01
#define OUT_X_LSB 0x02
#define OUT_Y_MSB 0x03
#define OUT_Y_LSB 0x04
#define OUT_Z_MSB 0x05
#define OUT_Z_LSB 0x06
void Accel_init();
void Accel_clean();
int initI2cBus(char* bus, int address);
int * get_accel();

#endif