/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Standard includes. */

/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    PROJ_6_Test_1.c
 * @brief   Application entry point.
 */




/*******************************************************************************
 * Code
 ******************************************************************************/

/* User callback function for DMA transfer. */


/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

#include "peripherals.h"
#include "logger.h"
#include "timer.h"
#include "MKL25Z4.h"
#include "queue1.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"
#include "fsl_adc16.h"
#include "fsl_dac.h"
#include "clock_config.h"
#include "board.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "math.h"
#include "pin_mux.h"
#include "LED.h"
#include "dma.h"


//General Defines
#define true 1
#define false 0

#define BUFF_LENGTH 64

volatile uint8_t mode;
volatile uint8_t transfer_counter=0;

//ADC_DAC Defines
#define DEMO_ADC16_BASEADDR ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /* PTE20, ADC0_SE0 */
#define DEMO_DAC_BASEADDR DAC0
#define DEMO_ADC16_IRQn ADC0_IRQn
#define DEMO_ADC16_IRQ_HANDLER_FUNC ADC0_IRQHandler

#define DAC_CALC (((1/3.3)*4096)-1)  //Macro to calculate true voltage value
/* Initialize ADC16 & DAC */
static void DAC_ADC_Init(void);

volatile bool g_Adc16ConversionDoneFlag = false;
volatile uint32_t g_Adc16ConversionValue = 0;
adc16_channel_config_t g_adc16ChannelConfigStruct;


/* The software timer period. */
#define SW_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)

static void SwTimerCallback1(TimerHandle_t xTimer);


//Lookup Table
float look_up_table[50]= {2635.868164,2788.860596,2936.988281,3077.915527,3209.419189,3329.425781,3436.042236,3527.587402,3602.617676,3659.948730,3698.677490,3718.192383,3718.185791,3698.658203
                          ,3659.917236,3602.573730,3527.532471,3435.977295,3329.351074,3209.336426,3077.825928,2936.893311,2788.761475,2635.766846,2480.322510,2324.879639,2171.889893,2023.766113,1882.84460,
                          1751.347778,1631.349365,1524.742188,1433.207031,1358.188477,1300.868774,1262.152588,1242.650146,1242.669067,1262.209106,1300.962158,1358.317139,1433.369019,1524.934692,1631.569458
                          ,1751.592285,1883.109741,2024.047241,2172.182861,2325.179443,2480.625000
                         };

volatile int lookup=0;
volatile int lookup1=0;
float voltRead;


extern volatile uint32_t var;
volatile uint16_t n=0;

//Task_function Prototypes
static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);

//Circular Buffer Declaration
Q_T ADC,DSP;

//TImer handle
TimerHandle_t SwTimerHandle1 = NULL;

//Transient buffer definition
extern uint32_t srcAddr[BUFF_LENGTH];
extern uint32_t destAddr[BUFF_LENGTH];


//Reference Voltage and register values
#define VREF_BRD 3.300
#define SE_12BIT 4096.0


//counter for the number of reports printed
volatile int report_count=0;

//Log levels- defines the verbosity of the logger
enum log_level levels=0;

//math Function Definitions from GeeksforGeeks
int largest(uint32_t arr[], int n);
int Smallest(uint32_t arr[], int n);
int Mean(uint32_t array[], int n);
float calculateSD(float data[]);


//Function to Initialize DAC pin write and ADC Pin Read
static void DAC_ADC_Init(void)
{
    adc16_config_t adc16ConfigStruct;
    dac_config_t dacConfigStruct;

    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
    DAC_Enable(DEMO_DAC_BASEADDR, true); /* Enable output. */
    ADC16_GetDefaultConfig(&adc16ConfigStruct);

#if defined(BOARD_ADC_USE_ALT_VREF)
    adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
#endif
    ADC16_Init(DEMO_ADC16_BASEADDR, &adc16ConfigStruct);

    /* Make sure the software trigger is used. */
    ADC16_EnableHardwareTrigger(DEMO_ADC16_BASEADDR, false);
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
    if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASEADDR))
    {
        PRINTF("\r\nADC16_DoAutoCalibration() Done.");
    }
    else
    {
        PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
    }
#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */

    /* Prepare ADC channel setting */
    g_adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
    g_adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = true;

#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
    g_adc16ChannelConfigStruct.enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
}


void DEMO_ADC16_IRQ_HANDLER_FUNC(void)
{
    g_Adc16ConversionDoneFlag = true;
    /* Read conversion result to clear the conversion completed flag. */
    g_Adc16ConversionValue = ADC16_GetChannelConversionValue(DEMO_ADC16_BASEADDR, DEMO_ADC16_CHANNEL_GROUP);
}


static void write_task_2(void *pvParameters);

//Function to retuen DAC register value in response to the reference voltage
float DAC_REGISTER_VALUES(float x)
{

    float sin_value=sinf(x*1.25667)+2;
    PRINTF("sin_value=%\n\r",sin_value);

    float dac_register_value=DAC_CALC*sin_value;
    PRINTF("DAC_REGISTER_VALUE=%f\n\r",dac_register_value);
    return dac_register_value;

}



/*******************************************************************************

 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */


int main(void)
{

    TimerHandle_t SwTimerHandle1 = NULL;
    /* Init board hardware. */


    //Intialize pins clock peripherals debug console ADC and IRQ
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    EnableIRQ(DEMO_ADC16_IRQn);
    DAC_ADC_Init();


    SystemCoreClockUpdate();
    LED_RED_INIT(1);
    LED_GREEN_INIT(1);   			//INITIALIZING THE LED INIT PINS
    LED_BLUE_INIT(1);

    //Initialize the ADC and DSP Buffer
    Q_Init(&ADC);
    Q_Init(&DSP);



#ifdef debug_mode
    mode = 1;									//flags set during various test modes
    levels=debug;

#endif

#ifdef status_mode
    mode = 2;									//flags set during various test modes
    levels=status;
#endif



///////////////Test Program to check the Buffers functioning Properly/////////////////

logger(post_test,status,true);
    PRINTF("\nADC Empty - %d        DSP Empty - %d   ",Q_Empty(&ADC),Q_Empty(&DSP));
    if(Q_Empty(&ADC))
        logger(buffer_empty,status,true);
    else
        logger(buffer_empty,status,false);
    int val = Q_Dequeue(&ADC);
    logger(buffer_empty,debug,true);
    PRINTF("\n\n --------- %f  %f  ",val,1);

    //Try adding dummy values and see if they pop and status of empty and full flags
    Q_Enqueue(&ADC,10);
    Q_Enqueue(&ADC,10);
    Q_Enqueue(&ADC,10);
    Q_Enqueue(&ADC,10);

    PRINTF("\nADC full - %d        DSP full - %d   ",Q_Full(&ADC),Q_Full(&DSP));
    if (Q_Full(&ADC))
        logger(buffer_full,status,true);
    else
        logger(buffer_full,status,false);

////////////////////////////////Test Program End /////////////////////////////////////



//Create a Software timer from FREE RTOS
    SwTimerHandle1 = xTimerCreate("SwTimer1",          /* Text name. */
                                  SW_TIMER_PERIOD_MS, /* Timer period. */
                                  pdTRUE,             /* Enable auto reload. */
                                  0,                  /* ID is not used. */
                                  SwTimerCallback1);   /* The callback function. */
//Start the Timer
    xTimerStart(SwTimerHandle1,0);

//Create the Tasks that create
    xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 400, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 400, NULL, tskIDLE_PRIORITY + 1, NULL);


 //////////////////////////////////Test Program 2 to check DMA Transfer////////////////////////


    int transfer_counter = 0;

    for (int i=0; i<=63; i++)
        srcAddr[i]=i;

    for (int j=0; j<=63; j++)
        destAddr[j]=0;

    for (transfer_counter=0; transfer_counter<=BUFF_LENGTH; transfer_counter++)
    {
        srcAddr[transfer_counter] = Q_Dequeue(&ADC);
    }

    DMA_func();

    for (transfer_counter=0; transfer_counter<=BUFF_LENGTH; transfer_counter++)
    {
        Q_Enqueue(&DSP,destAddr[transfer_counter]);
    }
    PRINTF("\n POST Successful\n");
    PRINTF("\n ----------------------POST TEST Program Over------------------------------\n");
    //////////////////////////////////Test Program 2 end////////////////////////


    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Software timer callback.
 */



static void SwTimerCallback1(TimerHandle_t xTimer)
{

    n++;

}

// Function to delay a for n milliseconds
void delay(uint16_t n)
{
    int i;
    for(i=0; i<n; i++)
    {
        for (i=0; i<1275; i++);
    }
}





//Task 1
/*	This task is responsible for reading the value from the lookup table every 100 millisecond
 *  and then putting it in the DAC0 register which is then output to the pin
 *
 */

static void write_task_1(void *pvParameters)
{
    while(1)
    {
    	 vTaskDelay(39.5);
         if (report_count==5)
         {
        	 vTaskDelay(100);
             vTaskEndScheduler();
             vTaskSuspend(NULL);

             vTaskDelete(write_task_1);
         }
        int y=lookup1%50;

        LED_GREEN_TOGGLE();
        LED_BLUE_OFF();
        DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, look_up_table[y]);
        lookup1++;
        //End task after 5 reports


    }

}




//Task 2
/*	This task is responsible for reading the value from the ADC and then putting it in
 * 	the Circular Buffer. When the Buffer is full then initiate the DMA transfer
 *
 */
static void write_task_2(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(39.5);

        ADC16_SetChannelConfig(DEMO_ADC16_BASEADDR, DEMO_ADC16_CHANNEL_GROUP, &g_adc16ChannelConfigStruct);

        while (!g_Adc16ConversionDoneFlag)
        {
        }


        //Get the Value from te ADC and Put in Variable var
        var=g_Adc16ConversionValue;
        logger(adc_register_value,debug,var);

        //Add the value read from ADC into ADC buffer
        Q_Enqueue(&ADC,var);

        //If the Buffer is full then initiate the DMA Transfer
        if(Q_Full(&ADC))
        {
            timer();
            report_count++;
            for(int i=0; i<=BUFF_LENGTH; i++)
            {
                srcAddr[i]=Q_Dequeue(&ADC);

            }
            LED_BLUE_TOGGLE();
            LED_GREEN_OFF();
            delay(500);
            timer();

            //Call the DMA
            DMA_func();
            logger(destination_buffer_print,status,true);

            for (int j=0; j<=BUFF_LENGTH; j++)
            {

                logger(report,debug,destAddr[j]);


            }
        }

        //Stop the tasks after 5 report prints
        if (report_count==5)
        {
            vTaskEndScheduler();
            vTaskDelay(100);
            vTaskDelete(write_task_2);
        }
    }


}



