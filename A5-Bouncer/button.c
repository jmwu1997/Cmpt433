// c file for button/joystick
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"
#include "uart_irda_cir.h"
#include "consoleUtils.h"
#include <stdint.h>
#include "button.h"



/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
// Boot btn on BBB: SOC_GPIO_2_REGS, pin 8
// Down on Zen cape: SOC_GPIO_1_REGS, pin 14  NOTE: Must change other "2" constants to "1" for correct initialization.
// Left on Zen cape: SOC_GPIO_2_REGS, pin 1
// --> This code uses left on the ZEN:
// #define BUTTON_GPIO_BASE     (SOC_GPIO_2_REGS)
// #define BUTTON_PIN           (1)
#define BUTTON_BASE_UP     (SOC_GPIO_0_REGS)
#define BUTTON_BASE_DOWN   (SOC_GPIO_1_REGS)
#define BUTTON_BASE_LEFT   (SOC_GPIO_2_REGS)

#define BUTTON_PIN_UP      (26)
#define BUTTON_PIN_DOWN    (14)
#define BUTTON_PIN_LEFT    (1)

#define DELAY_TIME 0x4000000

#define BAUD_RATE_115200          (115200)
#define UART_MODULE_INPUT_CLK     (48000000)


/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
// static void initializeButtonPin(void);
// static _Bool readUp(void);
// static _Bool readDown(void);
// static _Bool readLeft(void);
// static _Bool readButtonWithBitTwiddling(void);

// static void uartInitialize(void);
// static void uartBaudRateSet(void);


/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/
#include "hw_cm_per.h"
void GPIO2ModuleClkConfig(void)
{

    /* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
          CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
           CM_PER_GPIO2_CLKCTRL_MODULEMODE));
    /*
    ** Writing to OPTFCLKEN_GPIO_2_GDBCLK bit in CM_PER_GPIO2_CLKCTRL
    ** register.
    */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
          CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK;

    /*
    ** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
    ** value.
    */
    while(CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
           CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK));

    /*
    ** Waiting for IDLEST field in CM_PER_GPIO2_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_GPIO2_CLKCTRL_IDLEST_FUNC <<
           CM_PER_GPIO2_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
            CM_PER_GPIO2_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_GPIO_2_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK));
}

void initializeButtonPin(void)
{
    /* Enabling functional clocks for GPIO2 instance. */
  	GPIO0ModuleClkConfig();
  	GPIO1ModuleClkConfig();
  	GPIO2ModuleClkConfig();

    /* Selecting GPIO1[23] pin for use. */
    //GPIO1Pin23PinMuxSetup();

    /* Enabling the GPIO module. */
    //GPIOModuleEnable(BUTTON_GPIO_BASE);
    GPIOModuleEnable(BUTTON_BASE_DOWN);
    GPIOModuleEnable(BUTTON_BASE_UP);
    GPIOModuleEnable(BUTTON_BASE_LEFT);

    /* Resetting the GPIO module. */
    //GPIOModuleReset(BUTTON_GPIO_BASE);
    GPIOModuleReset(BUTTON_BASE_UP);
    GPIOModuleReset(BUTTON_BASE_DOWN);
    GPIOModuleReset(BUTTON_BASE_LEFT);

    /* Setting the GPIO pin as an input pin. */
    // GPIODirModeSet(BUTTON_GPIO_BASE,
    //                BUTTON_PIN,
    //                GPIO_DIR_INPUT);
    GPIODirModeSet(BUTTON_BASE_LEFT,
                   BUTTON_PIN_LEFT,
                   GPIO_DIR_INPUT);
    GPIODirModeSet(BUTTON_BASE_UP,
                   BUTTON_PIN_UP,
                   GPIO_DIR_INPUT);
    GPIODirModeSet(BUTTON_BASE_DOWN,
                   BUTTON_PIN_DOWN,
                   GPIO_DIR_INPUT);
}

// return directions in int value
int readDirections(){
  if (readLeft()){
    return BUTTON_PIN_LEFT;
  }
  else if (readUp()){
    return BUTTON_PIN_UP;
  }
  else if (readDown()){
    return BUTTON_PIN_DOWN;
  } 
  else return 0;
}

//check directions is pressed
_Bool readLeft(void)
{
	return GPIOPinRead(BUTTON_BASE_LEFT, BUTTON_PIN_LEFT) == 0;
}
_Bool readUp(void)
{
	return GPIOPinRead(BUTTON_BASE_UP, BUTTON_PIN_UP) == 0;
}
_Bool readDown(void)
{
	return GPIOPinRead(BUTTON_BASE_DOWN, BUTTON_PIN_DOWN) == 0;
}

// static _Bool readButtonWithBitTwiddling(void)
// {
// 	uint32_t regValue = HWREG(BUTTON_BASE_LEFT + GPIO_DATAIN);
// 	uint32_t mask     = (1 << BUTTON_PIN_LEFT);

// 	return (regValue & mask) == 0;
// }



// void uartInitialize()
// {
// 	/* Configuring the system clocks for UART0 instance. */
// 	UART0ModuleClkConfig();
// 	/* Performing the Pin Multiplexing for UART0 instance. */
// 	UARTPinMuxSetup(0);
// 	/* Performing a module reset. */
// 	UARTModuleReset(SOC_UART_0_REGS);
// 	/* Performing Baud Rate settings. */
// 	uartBaudRateSet();
// 	/* Switching to Configuration Mode B. */
// 	UARTRegConfigModeEnable(SOC_UART_0_REGS, UART_REG_CONFIG_MODE_B);
// 	/* Programming the Line Characteristics. */
// 	UARTLineCharacConfig(SOC_UART_0_REGS,
// 			(UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1),
// 			UART_PARITY_NONE);
// 	/* Disabling write access to Divisor Latches. */
// 	UARTDivisorLatchDisable(SOC_UART_0_REGS);
// 	 Disabling Break Control. 
// 	UARTBreakCtl(SOC_UART_0_REGS, UART_BREAK_COND_DISABLE);
// 	/* Switching to UART16x operating mode. */
// 	UARTOperatingModeSelect(SOC_UART_0_REGS, UART16x_OPER_MODE);
// 	/* Select the console type based on compile time check */
// 	ConsoleUtilsSetType(CONSOLE_UART);
// }


// /*
//  ** A wrapper function performing Baud Rate settings.
//  */
// static void uartBaudRateSet(void)
// {
// 	unsigned int divisorValue = 0;

// 	/* Computing the Divisor Value. */
// 	divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
// 			BAUD_RATE_115200,
// 			UART16x_OPER_MODE,
// 			UART_MIR_OVERSAMPLING_RATE_42);

// 	/* Programming the Divisor Latches. */
// 	UARTDivisorLatchWrite(SOC_UART_0_REGS, divisorValue);
// }


