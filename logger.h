#ifndef _LOGGER_H_
#define _LOGGER_H_
#include<stdint.h>
#include <stdio.h>



#define true 1                   //boolean
#define false 0

enum log_level{
	test,
	status,
	debug                  			//enum for various test modes
};
enum function_type{

	receiver_available,         //enum defined for various function call print messages
	receive_char,
	receiver_poll,
	transmitter_available,
	transmitter_char,
	transmitter_poll,
	transmitter_int,
	receiver_int,
	uart_interrupt,
	uart_poll,
	uart_init,
	uart_application,
	uart_echo,
	buffer_full,
	buffer_empty,
	buffer_valid,
	buffer_init,
	buffer_destroy,
	buffer_add,
	buffer_remove,
	all
};

void logger(enum function_type functions,enum log_level level,uint8_t message);
void log_uart_mode_int(enum function_type functions);
void log_uart_mode_poll(enum function_type functions);
void log_uart_init(enum function_type functions);
void log_uart_echo(enum function_type functions);
void log_uart_application(enum function_type functions);
void log_buffer_full(enum function_type functions,enum log_level level);
void log_buffer_empty(enum function_type functions,enum log_level level);
void log_buffer_valid(enum function_type functions,enum log_level level,uint8_t message);
void log_buffer_init(enum function_type functions,enum log_level level);
void log_buffer_add(enum function_type functions,enum log_level level,uint8_t message);
void log_buffer_remove(enum function_type functions,enum log_level level,uint8_t message);
void log_buffer_destory(enum function_type functions,enum log_level level);

#endif
