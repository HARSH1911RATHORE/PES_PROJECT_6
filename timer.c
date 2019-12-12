#include "timer.h"
#include"logger.h"
#include"fsl_debug_console.h"

//enum function_type functions=0;
volatile uint16_t x=0;

void timer()
{
    x=n*20;
    mili_value=x%100;
    second_value=x/100;
    seconds=second_value%60;                           //printing the timer value form the start of program
    minutes=second_value/60;
      PRINTF("\n\r");
    logger(timers,debug,minutes);				//converting integer to integer string
    PRINTF(":");
    logger(timers,debug,seconds);
    PRINTF(":");
    logger(timers,debug,mili_value);
    PRINTF("\n\r");


}
