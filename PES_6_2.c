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


/*
 * @brief   Application entry point.
 */



















 #include "peripherals.h"


#include "MKL25Z4.h"


#include "queue1.h"

#include "fsl_dma.h"
#include "fsl_dmamux.h"

#include "fsl_adc16.h"
#include "fsl_dac.h"

#include "clock_config.h"




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


#define DMA_CHANNEL 0
#define DMA_SOURCE 63
#define BUFF_LENGTH 64

dma_handle_t g_DMA_Handle;
volatile bool g_Transfer_Done = false;



#define DEMO_ADC16_BASEADDR ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /* PTE20, ADC0_SE0 */
#define DEMO_DAC_BASEADDR DAC0

#define DEMO_ADC16_IRQn ADC0_IRQn
#define DEMO_ADC16_IRQ_HANDLER_FUNC ADC0_IRQHandler
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The software timer period. */
#define SW_TIMER_PERIOD_MS (100 / portTICK_PERIOD_MS)
//#define SW_TIMER_PERIOD_MS2 (10000 / portTICK_PERIOD_MS)

#define DAC_CALC (((1/3.3)*4096)-1)
volatile static uint16_t n=0;
volatile static uint16_t x=0;							//all the variables needed to calculate timer value
volatile static uint16_t mili_value=0;
volatile static uint16_t second_value=0;
volatile static uint16_t seconds=0;
volatile static uint16_t minutes=0;
volatile static uint16_t min[100];
volatile static uint16_t sec[100];
volatile static uint16_t mil[100];
float look_up_table[52]={2480.424316,2635.868164,2788.860596,2936.988281,3077.915527,3209.419189,3329.425781,3436.042236,3527.587402,3602.617676,3659.948730,3698.677490,3718.192383,3718.185791,3698.658203
		,3659.917236,3602.573730,3527.532471,3435.977295,3329.351074,3209.336426,3077.825928,2936.893311,2788.761475,2635.766846,2480.322510,2324.879639,2171.889893,2023.766113,1882.84460,
		1751.347778,1631.349365,1524.742188,1433.207031,1358.188477,1300.868774,1262.152588,1242.650146,1242.669067,1262.209106,1300.962158,1358.317139,1433.369019,1524.934692,1631.569458
		,1751.592285,1883.109741,2024.047241,2172.182861,2325.179443,2480.625000};

volatile int lookup=0;
float voltRead;
volatile uint8_t dma=0;

volatile uint8_t transfer_counter=0;

volatile uint32_t var;
static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);
//static void write_task_3(void *pvParameters);

Q_T ADC,DSP;

TimerHandle_t SwTimerHandle1 = NULL;
uint32_t srcAddr[BUFF_LENGTH];
uint32_t destAddr[BUFF_LENGTH];

dma_transfer_config_t transferConfig;

#define VREF_BRD 3.300
#define SE_12BIT 4096.0


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Initialize ADC16 & DAC */
static void DAC_ADC_Init(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool g_Adc16ConversionDoneFlag = false;
volatile uint32_t g_Adc16ConversionValue = 0;
adc16_channel_config_t g_adc16ChannelConfigStruct;

/*******************************************************************************
 * Code
 ******************************************************************************/



void DMA_Callback(dma_handle_t *handle, void *param)
{
    g_Transfer_Done = true;
}





static void DAC_ADC_Init(void)
{
    adc16_config_t adc16ConfigStruct;
    dac_config_t dacConfigStruct;

    /* Configure the DAC. */
    /*
     * dacConfigStruct.referenceVoltageSource = kDAC_ReferenceVoltageSourceVref2;
     * dacConfigStruct.enableLowPowerMode = false;
     */
    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
    DAC_Enable(DEMO_DAC_BASEADDR, true); /* Enable output. */

    /* Configure the ADC16. */
    /*
     * adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceVref;
     * adc16ConfigStruct.clockSource = kADC16_ClockSourceAsynchronousClock;
     * adc16ConfigStruct.enableAsynchronousClock = true;
     * adc16ConfigStruct.clockDivider = kADC16_ClockDivider8;
     * adc16ConfigStruct.resolution = kADC16_ResolutionSE12Bit;
     * adc16ConfigStruct.longSampleMode = kADC16_LongSampleDisabled;
     * adc16ConfigStruct.enableHighSpeed = false;
     * adc16ConfigStruct.enableLowPower = false;
     * adc16ConfigStruct.enableContinuousConversion = false;
     */
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



/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback1(TimerHandle_t xTimer);

//static void SwTimerCallback2(TimerHandle_t xTimer);
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

void Init_SysTick(void){                                  //systick timer set to 10 hz
	SysTick->LOAD = (48000000L/160*2);
	NVIC_SetPriority(SysTick_IRQn,3);
	SysTick->VAL=0;
	SysTick->CTRL=SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk;

}

//void SysTick_Handler()
//{
//n++;
//}
void timer()
{
	x=n*20;
	mili_value=x%100;
	second_value=x/100;
	seconds=second_value%60;                           //printing the timer value form the start of program
	minutes=second_value/60;
	PRINTF("\n\r");
	PRINTF("0:");
	PRINTF("%d", minutes);
	sprintf((char *) min, "%d", minutes);            //converting integer to integer string
//	PRINTF(min);
	PRINTF(":");
	PRINTF("%d",seconds);
	sprintf((char *) sec, "%d",seconds );

	PRINTF(":");
	PRINTF("%d",mili_value);
	sprintf((char *) mil, "%d",mili_value );

	PRINTF("\n\r");


}
float DAC_REGISTER_VALUES(float x)
{

float sin_value=sinf(x*1.25667)+2;
PRINTF("sin_value=%\n\r",sin_value);

float dac_register_value=DAC_CALC*sin_value;
PRINTF("DAC_REGISTER_VALUE=%f\n\r",dac_register_value);
return dac_register_value;

}

void DMA()
{
    DMAMUX_Init(DMAMUX0);
    DMAMUX_SetSource(DMAMUX0, DMA_CHANNEL, DMA_SOURCE);
    DMAMUX_EnableChannel(DMAMUX0, DMA_CHANNEL);
    /* Configure DMA one shot transfer */
    DMA_Init(DMA0);
    DMA_CreateHandle(&g_DMA_Handle, DMA0, DMA_CHANNEL);
    DMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);
    DMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(srcAddr[0]), destAddr, sizeof(destAddr[0]), sizeof(srcAddr),
                        kDMA_MemoryToMemory);
    DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);
    DMA_StartTransfer(&g_DMA_Handle);
    /* Wait for DMA transfer finish */
}


int main(void)
{

//    TimerHandle_t SwTimerHandle2 = NULL;
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
// changhe   BOARD_InitBootPins();
//change    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    EnableIRQ(DEMO_ADC16_IRQn);

    DAC_ADC_Init();
    SystemCoreClockUpdate();






        /////////    Processing Here


        Q_Init(&ADC);
        Q_Init(&DSP);

//        Q_Enqueue(&ADC,10);


        PRINTF("\nADC Empty - %d        DSP Empty - %d   ",Q_Empty(&ADC),Q_Empty(&DSP));
        int val = Q_Dequeue(&ADC);
        PRINTF("\n\n --------- %f  %f  ",val,1);

        Q_Enqueue(&ADC,10);
        Q_Enqueue(&ADC,10);
        Q_Enqueue(&ADC,10);
        Q_Enqueue(&ADC,10);

        PRINTF("\nADC full - %d        DSP full - %d   ",Q_Full(&ADC),Q_Full(&DSP));



        SwTimerHandle1 = xTimerCreate("SwTimer1",          /* Text name. */
                                     SW_TIMER_PERIOD_MS, /* Timer period. */
                                     pdTRUE,             /* Enable auto reload. */
                                     0,                  /* ID is not used. */
                                     SwTimerCallback1);   /* The callback function. */
        xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 1, NULL);
        xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 2, NULL);

        /////////////////////////////////////DMA

        int transfer_counter = 0;

        for (int i=0; i<=63;i++)
        	srcAddr[i]=i;

        for (int j=0;j<=63;j++)
        	destAddr[j]=0;

        for (transfer_counter=0;transfer_counter<=BUFF_LENGTH;transfer_counter++)
        {
        	srcAddr[transfer_counter] = Q_Dequeue(&ADC);
        }


    //    	uint32_t srcAddr[BUFF_LENGTH] = {0x01, 0x02, 0x03, 0x04};
    //        uint32_t destAddr[BUFF_LENGTH] = {0x00, 0x00, 0x00, 0x00};

    //    	 uint32_t srcAddr = &ADC;
    //       uint32_t destAddr = &DSP;



            uint32_t i = 0;



            /* Print source buffer */
            PRINTF("DMA memory to memory transfer example begin.\r\n\r\n");
            PRINTF("Destination Buffer:\r\n");
            for (i = 0; i < BUFF_LENGTH; i++)
            {
                PRINTF("%d\t", destAddr[i]);
            }
            /* Configure DMAMUX */
            DMAMUX_Init(DMAMUX0);
            DMAMUX_SetSource(DMAMUX0, DMA_CHANNEL, DMA_SOURCE);
            DMAMUX_EnableChannel(DMAMUX0, DMA_CHANNEL);
            /* Configure DMA one shot transfer */
            DMA_Init(DMA0);
            DMA_CreateHandle(&g_DMA_Handle, DMA0, DMA_CHANNEL);
            DMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);
            DMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(srcAddr[0]), destAddr, sizeof(destAddr[0]), sizeof(srcAddr),
                                kDMA_MemoryToMemory);
            DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);
            DMA_StartTransfer(&g_DMA_Handle);
            /* Wait for DMA transfer finish */

 //           xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 1, NULL);


//            while (g_Transfer_Done != true)
//            {
//            }
//            /* Print destination buffer */
//            PRINTF("\r\n\r\nDMA memory to memory transfer example finish.\r\n\r\n");


            PRINTF("Destination Buffersssssssss:\r\n");
            for (i = 0; i < BUFF_LENGTH; i++)
            {
                PRINTF("%d\t", destAddr[i]);
            }


            for (transfer_counter=0;transfer_counter<=BUFF_LENGTH;transfer_counter++)
                {
            		 Q_Enqueue(&DSP,destAddr[transfer_counter]);
                }






































//    float y=sinf(1.57);
//    PRINTF("SIN_VALUE=%.5f\n\r",y);
//    int x =DAC_CALC;
//    PRINTF("DAC_CALC= %d\n\r",x);
//    int i;
//    volatile float time=0;
//    for (i=0;i<50;i++)
//    {
//    	look_up_table[i]=DAC_REGISTER_VALUES(time);
//		time+=0.1;
//		PRINTF("values=%d\n\r",look_up_table[i]);
//    }


    //        xTaskCreate(write_task_3, "WRITE_TASK_3", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 3, NULL);

//    Init_SysTick();
    /* Create the software timer. */



//    SwTimerHandle2 = xTimerCreate("SwTimer2",          /* Text name. */
//                                 SW_TIMER_PERIOD_MS2, /* Timer period. */
//                                 pdTRUE,             /* Enable auto reload. */
//                                 0,                  /* ID is not used. */
//                                 SwTimerCallback2);   /* The callback function. */
    /* Start timer. */

//    xTimerStart(SwTimerHandle2, 0);





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
	int x=lookup%50;

	  DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, look_up_table[x]);
	  lookup++;
      g_Adc16ConversionDoneFlag = false;
      ADC16_SetChannelConfig(DEMO_ADC16_BASEADDR, DEMO_ADC16_CHANNEL_GROUP, &g_adc16ChannelConfigStruct);

      while (!g_Adc16ConversionDoneFlag)
      {
      }
      PRINTF("\r\n\r\nADC Value: %d\r\n", g_Adc16ConversionValue);
      var=g_Adc16ConversionValue;

 //     xTaskResumeFromISR();
 //     Q_Enqueue(&ADC,var);
 //     int val = Q_Dequeue(&ADC);
 //     PRINTF("Destination Buffer:\r\n");
//      for (int i = 0; i < BUFF_LENGTH; i++)
//      {
//          PRINTF("%d\t", destAddr[5]);
//      }

      if(Q_Full(&ADC))
      {dma=1;}
      	if (dma==1)
      	{
              PRINTF("Destination Buffer:\r\n");
              for (int i = 0; i < BUFF_LENGTH; i++)
              {
                  PRINTF("%d\t", destAddr[i]);
              }
           PRINTF("DMAaaaaaa=%d",dma);
           dma=0;
           PRINTF("DMA=%d",dma);
      	}


      /* Convert ADC value to a voltage based on 3.3V VREFH on board */
      voltRead = (float)(g_Adc16ConversionValue * (VREF_BRD / SE_12BIT));
      PRINTF("\r\nADC Voltage: %0.3lf\r\n", voltRead);
    PRINTF("Tick.\r\n");
//    vTaskDelay()
 //   timer();

}

static void write_task_1(void *pvParameters)
{

	vTaskDelay(100);
	 xTimerStart(SwTimerHandle1, 0);
		vTaskSuspend(NULL);

}



//static void write_task_2(void *pvParameters)
//{
////	while(1)
////	{
////
////		vTaskDelay(100);
////		Q_Enqueue(&ADC,var);
////		PRINTF("hhhhhhhhhhhhhhhhhhhhhh%d   %d",Q_Dequeue(&ADC),Q_Empty(&ADC));
////		PRINTF("\n\r  Destination Buffer:\r\n");
////      for (int i = 0; i < BUFF_LENGTH; i++)
////      {
////        PRINTF("%d\t", destAddr[i]);
////      }
////    //  vTaskSuspend(NULL);
////}
//}






static void write_task_2(void *pvParameters)
{
	while(1)
	{
//		vTaskDelay(100);

//	PRINTF("hhhhh  %d",Q_Empty(&ADC));
	Q_Enqueue(&ADC,var);
	vTaskDelay(100);
	if (transfer_counter<64)
		{
		srcAddr[transfer_counter] = Q_Dequeue(&ADC);
		transfer_counter++;
		DMA();
		}

	else
	{
		for (int i=3;i<=66;i++)
				PRINTF("%d\t", destAddr[i]);

		transfer_counter=0;
	}

//	DMA();

//	vTaskDelay(100);
//	PRINTF("DESGFHDJS=%d",destAddr[5]);

//for (int i=0; i<=63;i++)
//{
//	PRINTF("DESGFHDJS=%d",destAddr[i]);
////	transfer_counter++;
//}
//transfer_counter=0;
//if
//for (int i=0;i<=63;i++)
//		PRINTF("%d\t", destAddr[i]);
if (dma==1)
{
	for (int i=0;i<=63;i++)
		PRINTF("%d\t", destAddr[i]);
}




}
}


//static void write_task_3(void *pvParameters)
//{
//	while(1)
//	{
//	if (dma==1)
//	{
//	PRINTF("\n\r  Destination Buffer:\r\n");
//	for (int i=0;i<=63;i++)
//	{
//		PRINTF(" %d\n\r",destAddr[i]);
//	}
//	//vTaskSuspend(NULL);
//	}
//	}
//}










//      }

//
//static void SwTimerCallback2(TimerHandle_t xTimer)
//{
//	if (dma==1)
//	{
////        PRINTF("Destination Buffer:\r\n");
////        for (int i = 0; i < BUFF_LENGTH; i++)
////        {
////            PRINTF("%d\t", destAddr[i]);
////        }
////     PRINTF("DMAaaaaaa=%d",dma);
////     dma=0;
////     PRINTF("DMA=%d",dma);
//	}
//}
/*******************************************************************************
 * Definitions
 ******************************************************************************/

//#define DAC_1_0_VOLTS 1241U
//#define DAC_1_5_VOLTS 1862U
//#define DAC_2_0_VOLTS 2482U
//#define DAC_2_5_VOLTS 3103U
//#define DAC_3_0_VOLTS 3724U



/*!
 * @brief Main function
 */








//
//        /* Set next state */
//        if (msg == '2')
//        {
//            DAC_Deinit(DEMO_DAC_BASEADDR);
//            ADC16_Deinit(DEMO_ADC16_BASEADDR);
//            break;
//        }


