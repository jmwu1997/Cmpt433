#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


#define gpioup "/sys/class/gpio/gpio26/value"
#define gpioright "/sys/class/gpio/gpio47/value"
#define gpiodown "/sys/class/gpio/gpio46/value"
#define gpioleft "/sys/class/gpio/gpio65/value"
#define led0 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define led1 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define led2 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define led3 "/sys/class/leds/beaglebone:green:usr3/brightness"




enum eJoystickDirections { Left, Right, Up, Down, nothing };

int readFromFileToScreen(char *fileName) {
	FILE *file = fopen( fileName, "r");
	if (file == NULL) {
		printf( "ERROR: Unable to open file (%s) for read\n", fileName);
		exit(-1);
	}
// Read string (line)
	const int max_length = 1024;
	char buff[max_length];
	fgets(buff, max_length, file);
// Close
	fclose(file);
	return atoi(buff);
}



void brightness(char *file ,int val) {

	FILE *pLedBrightnessFile = fopen(file, "w");
	if (pLedBrightnessFile == NULL) {
		printf("ERROR OPENING %s.", led0);
	}

	fprintf(pLedBrightnessFile,"%d", val);

	fclose(pLedBrightnessFile);
}

void nanosleeps() {
	for (int i = 0; i < 1; i++) {
		long seconds = 0;
		// 0.1 second = 1e+8 nano seconds
		long nanoseconds = 100000000;
		struct timespec reqDelay = {
				seconds,
				nanoseconds
		};
		nanosleep(&reqDelay, (struct timespec *) NULL);
	}
}


void flash(int times) {
	for (int i = 0; i< times; i++) {
		//turn on
		brightness(led0,1);
		brightness(led1,1);
		brightness(led2,1);
		brightness(led3,1);
		nanosleeps();
		//turn off
		brightness(led1,0);
		brightness(led2,0);
		brightness(led3,0);
		brightness(led0,0);
	        nanosleeps();
	}
}

void initialization() {
	// Use fopen() to open the file for write access.
	FILE *pfile1 = fopen(led0, "w");
	FILE *pfile2 = fopen(led1, "w");
	FILE *pfile3 = fopen(led2, "w");
	FILE *pfile4 = fopen(led3, "w");

	if (!pfile1 || !pfile2 || !pfile3 || !pfile4) {
		printf("ERROR: Unable to open export file.\n");
		exit(1);
	}
// Write to data to the file using fprintf():
	fprintf(pfile1,"%d", 0);
	fprintf(pfile2,"%d", 0);
	fprintf(pfile3,"%d", 0);
	fprintf(pfile4,"%d", 0);
// Close the file using fclose():
	fclose(pfile1);
	fclose(pfile2);
	fclose(pfile3);
	fclose(pfile4);

}

void clean() {
	//turn off all lights
	initialization();

}


enum eJoystickDirections getDirections() {
	if (readFromFileToScreen(gpioleft) == 0) {
		return Left;
	} else if(readFromFileToScreen(gpioright) == 0){	
		return Right;
	} else if(readFromFileToScreen(gpioup) == 0){
		return Up;
	} else if (readFromFileToScreen(gpiodown) == 0){
		return Down;
	} else {
		return nothing;
	}
}


int main(void) 
{
	printf("Hello embedded world, from Jia Ming!\n \nPress the Zen cape's Joystick in the direction of the LED.\n");
	printf("  UP for LED 0 (top)\n");
	printf("  DOWN for LED 3 (bottom)\n");
	printf("  LEFT/RIGHT for exit app.\n");

	initialization();
	int score = 0;
	int total = 0;
	srand(time(NULL));
	
	while(1) {
		printf("Press joystick; current score %d / %d \n", score,total);

		int r = rand() % 2;
		if (r == 1) {
			//turn on 0
			brightness(led0,1);
		} else {	
			//turn on 3
			brightness(led3,1);
		}

		enum eJoystickDirections directions = getDirections();

		while (directions == nothing) {
			if (directions == Up ||directions == Down){
				directions = nothing;
			}
			directions = getDirections();
		}
		if (directions == Left || directions == Right) {
			printf ("Your final score was (%d / %d)\n"
			"Thank you for playing!!\n", score , total);
			clean();
			exit(1);
		}
		if (((r == 1) && (directions == Up))||(r == 0 && directions == Down)) {
			printf("Correct!\n");
			flash(1);
			score++;
		} else {
			printf("Incorrect! :(\n");
			flash(5);
		}
		total ++;

	}
 	clean();

	return 0;
}
