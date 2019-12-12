
#include "logger.h"
#include "dma.h"

#include"fsl_debug_console.h"

volatile uint16_t mili_value=0;

volatile uint16_t seconds=0;
volatile uint16_t minutes=0;
volatile uint16_t mean=0;
volatile uint16_t large=0;            //EXTERN Variable defined for timer stopclock
volatile uint16_t small=0;
volatile float sd=0;
volatile uint32_t var=0;


void logger(enum function_type functions,enum log_level level,uint16_t message)
{

    log_large(functions,level,message);
    log_small(functions,level,message);
    log_mean(functions,level,message);                //different logger functions defined to print values and parameters as per the build target selected
    log_sd(functions,level,message);
    log_post_test(functions,level,message);
    log_adc(functions,level,message);

    log_report(functions,level,message);

    log_timer_min(functions,level,message);
    log_timer_seconds(functions,level,message);
    log_timer_mili(functions,level,message);




}

void log_post_test(enum function_type functions,enum log_level level,uint16_t message)            //post test
{
	if(functions==post_test)
		PRINTF("\n ----------------------POST TEST Program------------------------------\n");
}

void log_adc(enum function_type functions,enum log_level level,uint16_t message)                //printing the adc register value
{
    if(functions==adc_register_value)
    {
        if(message==var)

            PRINTF("\r\n\r\nADC Value: %d\r\n", message);
    }
}
void log_report(enum function_type functions,enum log_level level,uint16_t message)         //printing the report after 64 values
{
    if(functions==report)

        PRINTF("%d\t", message);
}

void log_large(enum function_type functions,enum log_level level,uint16_t message)       //printing the largest value of buffer
{
    if(functions==dma_call)
    {
        if(message==large)
            PRINTF(" Maximum Element = %d  \n",message);
    }
}

void log_small(enum function_type functions,enum log_level level,uint16_t message)      //printing the smallest value of buffer
{
    if(functions==dma_call)
    {
        if(message==small)
            PRINTF(" Smallest Element = %d  \n",message);
    }
}

void log_mean(enum function_type functions,enum log_level level,uint16_t message)     //printing the mean value of buffer
{
    if(functions==dma_call)
    {
        if(message==mean)
            PRINTF(" Mean = %d  \n",message);
    }

}
void log_sd(enum function_type functions,enum log_level level,uint16_t message)     //printing the standard deviation value of buffer
{
    if(functions==std)
    {
        if(message==sd)
            PRINTF("Standard Deviation  = %d  \n", message);
    }
}
void log_timer_min(enum function_type functions,enum log_level level,uint16_t message)  //minutes
{
    if(functions==timers)
    {
        if(level==debug)
        {
            if(message==minutes)
                PRINTF("%d",message);
        }
    }
}
void log_timer_seconds(enum function_type functions,enum log_level level,uint16_t message)  //seconds
{
    if(functions==timers)
    {
        if(level==debug)
        {
            if(message==seconds)
                PRINTF("%d",message);
        }
    }
}
void log_timer_mili(enum function_type functions,enum log_level level,uint16_t message)   //miliseconds
{
    if(functions==timers)
        if(level==debug)
        {
            if(message==mili_value)
                PRINTF("%d",message);
        }
}



