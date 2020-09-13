#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include "accelerometer.h"
int i2cFileDesc;

void Accel_init(){
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1,I2C_DEVICE_ADDRESS);
	unsigned char buff[2];
	buff[0] = WHO_AM_I;
	buff[1] = 0x01;
	write(i2cFileDesc, buff, sizeof(buff));

}

void Accel_clean(){
	unsigned char buff[2];
	buff[0] = WHO_AM_I;
	buff[1] = ZERO;
	write(i2cFileDesc, buff, sizeof(buff));
}

int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}

int * get_accel()
{
	
	// STATUS 	 0x00
	// OUT X MSB 0x01
	// OUT X LSB 0x02
	// OUT X MSB 0x03
	// OUT X LSB 0x04
	// OUT X MSB 0x05
	// OUT X LSB 0x06

	write(i2cFileDesc, ZERO, 1);
	unsigned char buff[7];
	static int array[3];
	if(read(i2cFileDesc, buff, sizeof(buff)) != 7)
	{
		printf("Error : Input/Output error \n");
		exit(1);
	}

	int16_t x = (buff[OUT_X_MSB] << 8) | (buff[OUT_X_LSB]);
		
	int16_t y = (buff[OUT_Y_MSB] << 8) | (buff[OUT_Y_LSB]);

	int16_t z = (buff[OUT_Z_MSB] << 8) | (buff[OUT_Z_LSB]);
	array[0] =x/10000;
	array[1]=y/10000;
	array[2]=z/10000;
	//x 0 stay left -1 right 1
	//y 0 stay in -1 out 1
	//z 1 stay side 0 down -1 
	return array;
}