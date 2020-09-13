// c file for the led displays
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"
#include "led.h"
#include "consoleUtils.h"


/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define LED_GPIO_BASE           (SOC_GPIO_1_REGS)
#define LED0_PIN (21)
#define LED1_PIN (22)
#define LED2_PIN (23)
#define LED3_PIN (24)

#define LED_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))

//#define DELAY_TIME 0x4000000		// Delay with MMU enabled
#define DELAY_TIME 0x40000		// Delay witouth MMU and cache
#define DEFAULT_SPEED 5
#define DEFAULT_PERIOD 16
#define BOUNCE 1
#define BAR 2
/*****************************************************************************
 **                INIT 
 *****************************************************************************/
static unsigned int period = DEFAULT_PERIOD;
static unsigned int current_speed=5;
static int pattern = BOUNCE;
static unsigned int counter = 0;
//0 1 2 3 2 1 0
//1 3 4 8 4 2 1
static int bounce[]= {0x1, 0x2, 0x4, 0x8, 0x4, 0x2, 0x1};
//0 01 012 0123 012 01 0
//1 3  7   15   7   3  1
static int bar[]= {0x1, 0x3, 0x7, 0xf, 0x7, 0x3, 0x1};
static int index = 0;

/*****************************************************************************
 **                INTERNAL FUNCTION PROTOTYPES
 *****************************************************************************/


/*****************************************************************************
 **                INTERNAL FUNCTION DEFINITIONS
 *****************************************************************************/


void initializeLeds(void)
{
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	/* Selecting GPIO1[23] pin for use. */
	//GPIO1Pin23PinMuxSetup();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(LED_GPIO_BASE);

	/* Resetting the GPIO module. */
	GPIOModuleReset(LED_GPIO_BASE);

	/* Setting the GPIO pin as an output pin. */
	GPIODirModeSet(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_DIR_OUTPUT);
}

void set_speed(unsigned int speed){
	if (speed < 0) {
		speed = 0;
	}
	if (speed > 9) {
		speed = 9;
	}
	period = 2 << (9 - speed);
	current_speed = speed;
}

void speed_up(){
	if(current_speed!=9){
		current_speed++;
	}
	period = 2 << (9 - current_speed);
}

void speed_down(){
	if(current_speed!=0){
		current_speed--;
	}
	period = 2 << (9 - current_speed);
}

void set_pattern(unsigned int new_pattern){
	pattern=new_pattern;
}

void switch_mode(void){
	if (pattern == 1){
		pattern = 2;
	}
	else if (pattern == 2){
		pattern = 1;
	}
}



/*

 ** The main function. Application starts here.
 */
// int main()
// {
// 	initializeLeds();
	
// 	bounce();

// 	while(0){
// 	Set_speed(9);
// 	speed_up();
// 	speed_down();
// 	bounce();
// 	}
// 	//driveLedsWithSWFunction();
// 	//driveLedsWithSetAndClear();
// 	//driveLedsWithBitTwiddling();
// }

void led_pattern(void){
	// Clear all the LEDs:
	HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = LED_MASK;
	//ConsoleUtilsPrintf("\npattern: %d\n",pattern);
	if(pattern == BOUNCE){
		HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = bounce[index] << LED0_PIN;
	}

	if(pattern == BAR){
		HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = bar[index] << LED0_PIN;
	}
	index++;
	// index 7 is one time cycle for testing only
	if(index==6){
		index=0;
	}
}


void led_flasher_counter(void)
{
	counter++;
	//set_speed(0);
	// ConsoleUtilsPrintf("\nspeed: %d\n",current_speed);
	// ConsoleUtilsPrintf("\nperiod: %d\n",period);
	if (counter >= period) {
		led_pattern();
		//reset counter 
		counter = 0;
	}
}





