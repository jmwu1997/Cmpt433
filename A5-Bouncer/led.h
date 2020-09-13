//Led.h:
//fucntions to change led displays and speed
#ifndef _LED_H_
#define _LED_H_

void initializeLeds(void);
void led_pattern(void);
void speed_up(void);
void speed_down(void);
void set_speed(unsigned int);
void set_pattern(unsigned int);
//static void driveLedsWithSWFunction(void);
//static void driveLedsWithSetAndClear(void);
//static void driveLedsWithBitTwiddling(void);
void led_flasher_counter(void);
void switch_mode(void);

#endif
