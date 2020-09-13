/*main loop*/
//Jia Ming Wu
//Levana He

//header files
#include "consoleUtils.h"
#include <stdint.h>

// My hardware abstraction modules
#include "serial.h"
#include "timer.h"
#include "wdtimer.h"

// My application's modules
//#include "fakeTyper.h"
#include "led.h"
#include "button.h"



/******************************************************************************
 **              Definitions and init variables
 ******************************************************************************/
#define BUTTON_PIN_UP      (26)
#define BUTTON_PIN_DOWN    (14)
#define BUTTON_PIN_LEFT    (1)
#define accepted_commands "Commands:\n"\
						  "? : Display this help message.\n"\
						  "0-9 : Set speed 0 (slow) to 9 (fast).\n"\
						  "a : Select pattern A (bounce).\n"\
						  "b : Select pattern B (bar).\n"\
						  "x : Stop hitting the watchdog.\n"

#define RESET_SOURCE_REG 0x44E00F00
#define PRM_RSTST 8
#define EXTERNAL_WARM_RST 5
#define WDT1_RST 4
#define GLOBAL_COLD_RST 0
static int lastButtonState;
static bool watchdogtrigger=true;
/******************************************************************************
 **              SERIAL PORT and LED HANDLING
 ******************************************************************************/
static volatile uint8_t s_rxByte = 0;
static void serialRxIsrCallback(uint8_t rxByte) {
	s_rxByte = rxByte;
}


static void doBackgroundSerialWork(void)
{
	if (s_rxByte != 0) {
		
		if (s_rxByte == '?') {
		ConsoleUtilsPrintf("\n");
		ConsoleUtilsPrintf(accepted_commands);
		}
		else if (s_rxByte == 'a') {
			ConsoleUtilsPrintf("\nChanging to bounce mode\n");
			// 1 is bounce mode 
			set_pattern(1);
		}
		else if (s_rxByte == 'b') {
			ConsoleUtilsPrintf("\nChanging to bar mode\n");
			// 2 is bar mode
			set_pattern(2);
		}
		else if (s_rxByte == 'x') {
			watchdogtrigger=false;
			ConsoleUtilsPrintf("\nNo longer hitting the watchdog.\n");
		}
		else if (s_rxByte >= '0' && s_rxByte <='9') {
			// take input and set speed
			set_speed(s_rxByte - '0');
			ConsoleUtilsPrintf("\nSetting LED speed to %d\n", s_rxByte - '0');
		}
		else {
			ConsoleUtilsPrintf("\nInvalid command.\n");
			ConsoleUtilsPrintf(accepted_commands);
		}
		s_rxByte = 0;
	}
}



static void LedIsrCallback(void)
{
	//put timer on led flasher.
	led_flasher_counter();

}


static void Button_doBackgroundWork(){
	if (lastButtonState != readDirections()) {
		if(readDirections()==0 && lastButtonState==BUTTON_PIN_UP){
			speed_up();
		}
		else if(readDirections()==0 && lastButtonState==BUTTON_PIN_DOWN){
			speed_down();
		}
		else if(readDirections()==0 && lastButtonState==BUTTON_PIN_LEFT){
			switch_mode();
		}
		lastButtonState = readDirections();
	}
}




/******************************************************************************
 **              MAIN
 ******************************************************************************/
int main(void)
{
	// Initialization
	initializeButtonPin();
	Serial_init(serialRxIsrCallback);
	Timer_init();
	Watchdog_init();
	initializeLeds();
	//uartInitialize();
	//initializeButtonPin();
	//FakeTyper_init();

	// Setup callbacks from hardware abstraction modules to application:
	Serial_setRxIsrCallback(serialRxIsrCallback);
	Timer_setTimerIsrCallback(LedIsrCallback);

	// Display startup messages to console:
	ConsoleUtilsPrintf("\nWelcome to Jia Ming and Levana's Flasher\n\n");
	ConsoleUtilsPrintf(accepted_commands);

	// Main loop:
	while(1) {
		// Handle background processing
		doBackgroundSerialWork();
		Button_doBackgroundWork();


		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			if (watchdogtrigger){
				Watchdog_hit();
			}
		}
	}
	
}
