# PES_PROJECT_6 - FreeRTOS, ADC, DAC, DMA, DSP

## Contributors - Utkarsh Dviwedi and Harsh Rathore
 
# Problem Statement 


## Introduction

-In this final PES project, you will generate a sine wave signal on the KL25Z’s DAC, capture the signal with the KL25Z’s ADC, transfer collected data buffers via DMA to another data buffer, and perform basic DSP analysis of the signal. You will exercise lookup tables, buffers, ADC, DAC, DMA, math functions, and basic FreeRTOS task management. You will also use an oscilloscope or logic analyzer to verify output.

-This project has two deliverable programs, one for the confirmation of the creation of sine wave data for the DAC and the second for full functionality for reading, transferring, and analysis of that data.
The programs should be able to be demonstrated in debug and normal modes. In debug mode, comments on program steps should be provided along with any values needed to verify proper operations. In normal mode, much less verbose logging should be used.

**General**
There are details for this project that are left to you to decide, for instance, the buffer structure for the ADC data capture. Document the design decisions you make to meet the requirements of the project in the Observations section of the project README.

In the event of ANY error condition in any part of the project, set the LED to steady red.
For this project you may use all available SDK and FreeRTOS functions. You must use the SDK supplied FreeRTOS 9.0.0 libraries and include files.

Your code should follow the ESE C Style Guide as closely as possible.

When compiling use -Wall and -Werror compiler flags. Your code should have no compilation errors or warnings.

You should use modular design for your code – drivers in one module, application code in another, etc.

## Program 1 – Lookup Table Creation and Validation

Calculate and create a lookup table to represent the values in a sine wave that runs from 1V to 3V. Period from peak to peak should be 5 seconds with a .1 second step. In code, mathematically determine the voltage values at each .1 second step. Convert the voltages to DAC register values representing the voltages at each .1 second step. Store the DAC register values in a buffer of sufficient size to hold all the values for a single peak to peak cycle.

Create a test case that uses a FreeRTOS Software Timer Callback function to apply the values from the lookup table to DAC0_OUT (pin J10-11) every .1 second, repeating from the beginning of the table once the last value is applied. Toggle a Blue LED on and off for each visit to the timer callback.

Confirm the shape of the output on a scope; voltages should be 1V to 3V, peak to peak period should be 5 seconds, signal should be a clear sine wave. Capture a scope image for submission (label Program 1).
When run in debug, program steps and values should be echoed to the console by logger statements.


## Program 2 – Capture the output from DAC0 on ADC0 (pin J10-1, ADC0_SE8)

Initialize this program by using the code from Program 1 to create a lookup table for use by the DAC FreeRTOS task, putting the register values that define the sine wave into a DAC value buffer of appropriate size.

Create and start the following FreeRTOS tasks. Note that you will need to determine appropriate task priorities and scheduling, as well as any need for mutex or semaphore use for communication or resource sharing, in order for the program to meet its performance goals.

Create a FreeRTOS Task to periodically (every .1 seconds) change the value on DAC0 from the lookup table buffer of DAC register values. Toggle a Green LED on and off for each DAC write (if the LED is available for use).

Create a FreeRTOS Task to periodically (every .1 seconds) read the DAC0 value via ADC0 and store it in a circular buffer. The ADC buffer will be 64 samples long and should contain the raw ADC register values from each read. When the buffer is full initiate a DMA transfer from the ADC buffer to a second buffer (called the DSP buffer). When the DMA Transfer is about to start, toggle the LED to Blue for .5 seconds. During this period, the LED cannot be used by other tasks. Clear (or overwrite) the ADC buffer with incoming DAC values and continue sampling until the next series of samples are collected. Capture a time stamp at the start and completion of the DMA transfer. You will need to consider the size and data width requirements for the ADC buffer and the DSP buffer.

Create a FreeRTOS Task that is triggered by an interrupt from completing the DMA transfer of data into the DSP buffer. Calculate the following floating point values from the ADC register values: maximum, minimum, average, and standard deviation of voltage levels. You should see approx. 2% error in the ADC reading the incoming DAC values. Report those values along with an incremented run number starting at 1 and the start time and end time for the last DMA transfer. Once run number 5 is completed and reported, terminate the DAC and ADC tasks, and terminate this task to end the program.

Confirm the shape of the ADC values on a scope; voltages should be very nearly 1V to 3V, peak to peak period should be 5 seconds, signal should be a clear sine wave. Capture a scope image for submission (label Program 2).

When run in debug, program steps and values should be echoed to the console by logger statements.

## Description of Repository contents

 1. **PES_PROJECT_6.c**   --Contains the main function , UART, Functions and Initializations
 2. **Timer.c**  --Contains the log time stamps  
 3. **logger.c**   --Contains the Logger functions 
 4. **logger.h** -- contains logger enum declarations
 5. **queue.c** --Contains all functions needed to implement circular buffer
 6. **queue.h** --Contains the declarations for circular buffer code
 7. **LED.c** --Contains all the functions and initialisation requried for the LED to function
 8. **LED.h** --Contains all declarations for led.c
 9. **dma.c** -- Containsc all functions and interrupts for dma
 10. **dma.h** -- Contains all header files and variables defined for dma
 11. **TIMER_CALLBACK_FUNCTION.c** --contains the adc and dac values to be loaded using timer callback interrupt
## OBSERVATIONS

 1. **FREE rtos task functions** - The freertos task create, suspend, delay functions were performed later as the timer calling function used to print values of lookup table into the buffer. The timer calling function was disabled after start, when it is called to feed values of DAC from lokkup table.
 2. **Logger** - Implementing the logger function to print the status of test, debug and status mode turned out to be very time consuming as all the functions messages were called by a single logger function with the use of loggers for different states. Also had to define #if for each finction to print the messages of them in different modes as needed by the user.
 3. **DMA** - The DMA operation when performed did not shift values of ADC into the ADC buffer. However, by hardcoding values, the buffer DMA operation was performed. Later realised that Dma start functions were not being used when accepting the adc values into the buffer.



## Installation and Execution Notes
 1. **LED INIT and MUX** - We need to initialize the GPIO of the LEDs and directions of PTB18(Red LED) ,PTB19(Green LED) and PTBD1(Blue LED)
 2. **Set the logger Mode flags** -There are global Flags for logger, Set the debug flag to 1 to run it in the debug mode and various global flags for printing the report of the dma, transfer_counter, length of queue.
 3. **Logger messagez** - The logger messages are printed using the uart driver functions
 4. **Defines** - #defines and #if are used at various places to run a particular mode and to print messages for a particular state.
 5. **DMA**-DMA is performed by using 2 buffers, source and destination addresses. 
 
 **REFERENCE=sdk_free_rtos_examples**
