
#include "logger.h"




void logger(enum function_type functions,enum log_level level,uint8_t message)
{


	log_uart_mode_int(functions);                         //used an enum to call the logger messages of different
	//function calls in their debug mode,test mode and status mode
	log_uart_mode_poll(functions);

	log_uart_init(functions);

	log_uart_echo(functions);

	log_uart_application(functions);

	log_buffer_valid(functions,level,message);        //functions are called through enum flag when an appropriate build
	//target is selected
	log_buffer_init(functions,message);

	log_buffer_add(functions,level,message);

	log_buffer_remove(functions,level,message);

	log_buffer_destory(functions,message);


}


void log_uart_mode_int(enum function_type functions)
{
	if(functions==all || functions==uart_interrupt)                  //prints message if uart is in interrupt mode

		Send_String_Poll("\n\rUART INTERRUPT MODE\n\r");

}

void log_uart_mode_poll(enum function_type functions)				//prints message if uart is in polling mode
{
	if(functions==all || functions==uart_poll)

		Send_String_Poll("\n\rUART POLLING MODE\n\r");
}

void log_uart_init(enum function_type functions)				  //prints message for uart initialization
{
	if(functions==all || functions==uart_init)

		Send_String_Poll("\n\rUART INITIALIZATION\n\r");
}

void log_uart_echo(enum function_type functions)				//prints message if echo mode is defined
{
	if(functions==all || functions==uart_echo)

		Send_String_Poll("\n\rUART ECHO MODE\n\r");
}

void log_uart_application(enum function_type functions)			//prints message if application mode is defined
{
	if(functions==all || functions==uart_application)
		Send_String_Poll("\n\rUART APPLICATION MODE\n\r");
}

void log_buffer_valid(enum function_type functions,enum log_level level,uint8_t message)
{
	if(functions==all || functions==buffer_valid)				//check for validity of buffer
	{
		if (level==test)											//prints messages in debug,test and status mode
			Send_String_Poll("\n\rCHECK IF BUFFER POINTER IS VALID\n\r");
		else if(level==debug)
		{

			if (message==true)
			{
				Send_String_Poll("\n\rBUFFER POINTER IS VALID\n\r");      //check if buffer pointer is valid
			}
			else
			{
				Send_String_Poll("\n\rERROR:BUFFER POINTER IS INVALID\n\r");
			}
		}
		else
			Send_String_Poll("\n\rCHECK IF BUFFER POINTER IS VALID\n\r");  //print the status message for buffer validity

	}
}

void log_buffer_init(enum function_type functions,enum log_level level)
{
	if(functions==all || functions==buffer_init)               //check for buffer pointer correct initialisation
	{
		if (level==test)
			Send_String_Poll("\n\rINITIALIZE THE BUFFER\n\r");     //prints messages in debug,test and status mode

		else if(level==debug)
			Send_String_Poll("\n\rINITIALIZE THE BUFFER\n\r");
		else
			Send_String_Poll("\n\rINITIALIZE THE BUFFER\n\r");

	}
}

void log_buffer_add(enum function_type functions,enum log_level level,uint8_t message)
{
	if(functions==all || functions==buffer_add)              //print message to add elements to the buffer
	{
		if (level==test)
		{														//check if buffer is already full before adding elements
			Send_String_Poll("\n\rADD NEW BUFFER ELEMENT\n\r");  //prints messages in debug,test and status mode

		}
		else if(level==debug)
		{

			if (message==true)
				Send_String_Poll("\n\rBUFFER ADD SUCCESSFUL\n\r");      //if buffer is full, error will be given
			else
				Send_String_Poll("\n\rERROR:BUFFER IS ALREADY FULL\n\r");
		}
		else
			Send_String_Poll("\n\rADD NEW BUFFER ELEMENT\n\r");
	}

}

void log_buffer_remove(enum function_type functions,enum log_level level,uint8_t message)
{
	if(functions==all || functions==buffer_remove)                  //print message to remove elements to the buffer
	{
		if (level==test)
		{
			Send_String_Poll("\n\rREMOVE BUFFER OLDEST ELEMENT\n\r"); //check if buffer is already empty before adding elements

		}
		else if(level==debug)
		{

			if (message==false)
				Send_String_Poll("\n\rERROR:BUFFER IS ALREADY EMPTY\n\r"); //prints messages in debug,test and status mode
			else
				Send_String_Poll("\n\rBUFFER REMOVAL SUCCESSFUL\n\r"); //prints messages in debug,test and status mode
		}
		else
			Send_String_Poll("\n\rREMOVE BUFFER OLDEST ELEMENT\n\r");
	}
}

void log_buffer_destory(enum function_type functions,enum log_level level)
{
	if(functions==all || functions==buffer_destroy)
	{                                                           //print message to remove elements to the buffer
		if (level==test)
			Send_String_Poll("\n\rDESTROY THE BUFFER\n\r");        //prints messages in debug,test and status mode

		else if(level==debug)
			Send_String_Poll("\n\rDESTROY THE BUFFER\n\r");
		else
			Send_String_Poll("\n\rDESTROY THE BUFFER\n\r");

	}
}

