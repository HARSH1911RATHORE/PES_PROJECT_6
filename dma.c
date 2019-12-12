
#include "dma.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"
#include"logger.h"                                 //all include files needed for dma operation
#include"math.h"

// DMA defines
#define DMA_CHANNEL 0
#define DMA_SOURCE 63                              //dma source 63 selected as default
#define BUFF_LENGTH 64



uint32_t srcAddr[BUFF_LENGTH];
uint32_t destAddr[BUFF_LENGTH];                  //srcaddr and dest add defined for adc buffer and dsp buffer dma transfer
dma_handle_t g_DMA_Handle;

volatile uint8_t dma=0;

extern volatile uint16_t mean;
extern volatile uint16_t large;
extern volatile uint16_t small;                 //variables defined extern to access in other files
extern volatile float sd;

dma_transfer_config_t transferConfig;
 volatile bool g_Transfer_Done = false;

 //Function to Find the Largest element of an array
 int largest(uint32_t arr[], int n)
 {
     int i;

     // Initialize maximum element
     int max = arr[0];

     // Traverse array elements from second and
     // compare every element with current max
     for (i = 1; i <= n; i++)
         if (arr[i] > max)
             max = arr[i];

     return max;
 }


 //Function to find the Smallest Element of an array
 int Smallest(uint32_t arr[], int n)
 {
     int i;

     // Initialize maximum element
     int min = arr[0];

     // Traverse array elements from second and
     // compare every element with current max
     for (i = 1; i <= n; i++)
         if (arr[i] < min)
             min = arr[i];

     return min;
 }

 //Function to find the Mean of an array
 int Mean(uint32_t array[], int n)
 {
     int sum,loop;
     float avg;

     sum = avg = 0;

     for(loop = 0; loop <= n; loop++)
     {
         sum = sum + array[loop];
     }

     avg = (float)sum / loop;
     return avg;
 }

 //Function to Find the Standard deviation of element of an array
 float calculateSD(uint32_t data[])
 {
     float sum = 0.0, mean, SD = 0.0;
     int i;
     for (i = 0; i < 64; i++)
     {
         sum += data[i];
     }
     mean = sum / 64;
     for (i = 0; i < 64; ++i)
         SD += pow(data[i] - mean, 2);
     return sqrt(SD/64);
 }


//DMA Interupt service routine which is called when the DMA transfer if done.
void DMA_Callback(dma_handle_t *handle, void *param)
{
    g_Transfer_Done = true;  // Reset the DMA Completion Flag

    //////////////////////////Report Generation/////////////////////////////////////
    printf("\n ////////////////////////////////////Report Generation///////////////////////// \n");

    //TO find the Largest Element in the DSP Buffer
    large=largest(destAddr,63);
    logger(dma_call,debug, large); //invoke logger to print Largest Element in the DSP Buffer

    //TO find the Smallest Element in the DSP Buffer
    small=Smallest(destAddr,63);
    logger(dma_call,debug,small); //invoke logger to print Smallest Element in the DSP Buffer

    //TO find the mean of all elements in the DSP Buffer
    mean=Mean(destAddr,63);
    logger(dma_call,debug,mean);//invoke logger to print mean of all Elements in the DSP Buffer

    //TO find the standard deviation of all elements in the DSP Buffer
    sd=(calculateSD(destAddr));
    PRINTF("Standard Deviation  = %f  \n", sd);
  //  logger(18,1,sd);//invoke logger to print Standard definitions of all Elements in the DSP Buffer
    printf("\n ////////////////////////////////////Report Over///////////////////////// \n");



}



//Function to configure and initiate DMA Transfer
void DMA_func()
{
  //  logger(dma,status,true);
    DMAMUX_Init(DMAMUX0);
    DMAMUX_SetSource(DMAMUX0, DMA_CHANNEL, DMA_SOURCE);  //Selects the source and Channel of Dma Transfer
    DMAMUX_EnableChannel(DMAMUX0, DMA_CHANNEL);  //Enable the Channel of the DMA
    /* Configure DMA one shot transfer */
    DMA_Init(DMA0);                                        //Initialize the DMA
    DMA_CreateHandle(&g_DMA_Handle, DMA0, DMA_CHANNEL);    //Create a Handle to reference the DMa transfer
    DMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);    //Set the callback function that is triggered when the transfer is complete

    //Set the configuration register with source , destination address and sizes
    DMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(srcAddr[0]), destAddr, sizeof(destAddr[0]), sizeof(srcAddr),
                        kDMA_MemoryToMemory);
    DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);   //submit a request for Transfer
    DMA_StartTransfer(&g_DMA_Handle);   //start The Transfer
    /* Wait for DMA transfer finish */
}

