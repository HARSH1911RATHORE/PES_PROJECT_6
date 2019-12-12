#ifndef _LOGGER_H_
#define _LOGGER_H_
#include<stdint.h>
#include <stdio.h>



#define true 1                   //boolean
#define false 0

extern enum log_level
{

    status,
    debug                  			//enum for various test modes
} levels;
extern enum function_type
{
    adc_register_value,
    destination_buffer_print,
    dma_destination_add,
    report,
    clean_indentations,
    buffer_full,
    buffer_empty,
    buffer_valid,
    dma_call,
    minu,
    seco,
    milivalue,
    report_gen,
    report_over,
    timers,
	post_test,
    buffer_add,
    buffer_remove,
    std,
    all
} functions;


void log_adc(enum function_type functions,enum log_level level,uint16_t message);
void logger(enum function_type functions,enum log_level level,uint16_t message);
void log_post_test(enum function_type functions,enum log_level level,uint16_t message);
void log_report(enum function_type functions,enum log_level level,uint16_t message);
void log_large(enum function_type functions,enum log_level level,uint16_t message);
void log_small(enum function_type functions,enum log_level level,uint16_t message);
void log_mean(enum function_type functions,enum log_level level,uint16_t message);
void log_sd(enum function_type functions,enum log_level level,uint16_t message);
void log_timer_min(enum function_type functions,enum log_level level,uint16_t message);
void log_timer_seconds(enum function_type functions,enum log_level level,uint16_t message);
void log_timer_mili(enum function_type functions,enum log_level level,uint16_t message);




#endif
