#ifndef _SEGDISPLAY_H_
#define _SEGDISPLAY_H_


void writeToFile(char* fileName, char* value);
void Led_Clean();
void Led_Init();
void resetLed();

void *LightLed();
const int left_pin(int number);
const int right_pin(int number);
void timing();
void sleep1s();


#endif