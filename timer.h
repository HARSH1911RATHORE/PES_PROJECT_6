#ifndef _TIMER_H_
#define _TIMER_H_
#include<stdint.h>
#include <stdio.h>

extern volatile uint16_t n;
//all the variables needed to calculate timer value
extern volatile uint16_t mili_value;
volatile static uint16_t second_value=0;
extern volatile uint16_t seconds;
extern volatile uint16_t minutes;
volatile static uint16_t min[100];
volatile static uint16_t sec[100];
volatile static uint16_t mil[100];

void timer();

#endif
