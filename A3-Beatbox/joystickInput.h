#ifndef _JOYSTICK_CONTROLER_H_
#define _JOYSTICK_CONTROLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>			
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

//init function
void initJoystick();
//clean function
void cleanupJoystick();

#endif