//button.h
//button function definition to controll the joystick
#ifndef _BUTTON_H_
#define _BUTTON_H_

void initializeButtonPin(void);
_Bool readUp(void);
_Bool readDown(void);
_Bool readLeft(void);
//void uartInitialize(void);
int readDirections();


#endif