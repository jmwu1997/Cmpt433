#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "potDriver.h"
#include "segDisplay.h"
#include "sorter.h"


#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define export "/sys/class/gpio/export"
#define left_dir "/sys/class/gpio/gpio61/direction"
#define right_dir "/sys/class/gpio/gpio44/direction"
#define left_val "/sys/class/gpio/gpio61/value"
#define right_val "/sys/class/gpio/gpio44/value"

int left;
int right;
_Bool truth_val=true;
static pthread_t display_id;
static int i2cFileDesc;
static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
//static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);

void* LightLed()
{
    
	int prev_count = 0;
	while (truth_val) {

		//calculate the new val
		int curr_count = Sorter_getNumberArraysSorted();
		int new_count = curr_count - prev_count;
		prev_count = curr_count;
		//display numbers
		if (new_count > 99){
			left = 9;
			right = 9;
		}
		else{
		left = new_count/10;
		right = new_count%10;
		}
			//the output is updated every 1 second by the clock
		    clock_t begin;
			double time;
			 
			begin = clock();
			
			while(true){
				
				
				time=(double)(clock()-begin)/CLOCKS_PER_SEC;
				if(time >= 1.0){break;}
				//for every 1 second ,break
				
				timing();
				resetLed();
			    writeToFile(left_val, "1");
				writeI2cReg(i2cFileDesc, REG_OUTA, left_pin(left));
				writeI2cReg(i2cFileDesc, REG_OUTB, right_pin(left));
				
				timing();
				resetLed();
				writeToFile(right_val, "1");
				
				writeI2cReg(i2cFileDesc, REG_OUTA, left_pin(right));
				writeI2cReg(i2cFileDesc, REG_OUTB, right_pin(right));
			
				
			}
			
		//printf("count = %d, left =%d , right =%d\n",new_count,left, right);
	    	
	    
		}
		//output last reg address 

		writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
		writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
		resetLed();
		close(i2cFileDesc);
		return NULL;

	
}



void resetLed(){
		writeToFile(right_val, "0");
	    writeToFile(left_val, "0");
	
}


void timing() {
	for (int i = 0; i < 1; i++) {
		long seconds = 0;
		long nanoseconds = 5000000;
		struct timespec reqDelay = {
				seconds,
				nanoseconds
		};
		nanosleep(&reqDelay, (struct timespec *) NULL);
	}
}
void sleep1s() {
	for (int i = 0; i < 1; i++) {
		long seconds = 1;
		long nanoseconds = 0;
		struct timespec reqDelay = {
				seconds,
				nanoseconds
		};
		nanosleep(&reqDelay, (struct timespec *) NULL);
	}
}

//leftpin match with right pin gets the number
const int left_pin (int number) {
	switch (number){
		case 0:
			return 0xA1;
		case 1:
			return 0x80;
		case 2:
			return 0x31;
		case 3:
			return 0xB0;
		case 4:
			return 0x90;
		case 5:
			return 0xB0;
		case 6:
			return 0xB1;
		case 7:
			return 0x80;
		case 8:
			return 0xB1;
		case 9:
			return 0x90;
		default:
			return 0xA1;	
	}
}

const int right_pin (int number) {
	switch (number){
		case 0:
			return 0x86;
		case 1:
			return 0x02;
		case 2:
			return 0x0E;
		case 3:
			return 0x0E;
		case 4:
			return 0x8A;
		case 5:
			return 0x8C;
		case 6:
			return 0x8C;
		case 7:	
			return 0x06;
		case 8:
			return 0x8E;
		case 9:
			return 0x8E;
		default:
			return 0x86;
	}
}


void Led_Init(){
	//config pin
	system("config-pin P9_18 i2c");
	system("config-pin P9_17 i2c");
	//write all the val for init
	writeToFile(export, "61");
	writeToFile(export, "44");
	writeToFile(right_dir, "out");
	writeToFile(left_dir, "out");
	writeToFile(right_val, "1");
	writeToFile(left_val, "1");
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
	pthread_create(&display_id, NULL, &LightLed, NULL);
	
}
	

void Led_Clean() {
	//empty address and turn of lights
	truth_val=false;
	pthread_join(display_id,NULL);
}

void writeToFile(char* fileName, char* value) {
	FILE* pfile = fopen(fileName, "w");
	if (pfile == NULL) {
		printf("ERROR: Unable to open export file.\n");
		exit(1);
	}
	fprintf(pfile, "%s", value);
	// Write to data to the file using fprintf():
	fclose(pfile);
}



static int initI2cBus(char* bus, int address)
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

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}
/*
static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("Unable to write i2c register.");
		exit(-1);
	}

	// Now read the value and return it
	char value = 0;
	res = read(i2cFileDesc, &value, sizeof(value));
	if (res != sizeof(value)) {
		perror("Unable to read i2c register");
		exit(-1);
	}
	return value;
}*/
