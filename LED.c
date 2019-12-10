#include <MKL25Z4.H>

#include "LED.h"

#include "gpio_defs.h"



void Init_RGB_LEDs(void) {

	// Enable clock to ports B and D

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;



	// Make 3 pins GPIO

	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;             //set port for red led

	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);

	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          //set port for green  led

	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);

	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;			//set port for blue led

	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);



	// Set ports to outputs

	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);

	PTD->PDDR |= MASK(BLUE_LED_POS);

}



void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {

	if (red_on) {

			PTB->PCOR = MASK(RED_LED_POS);       //set red led on

	} else {

			PTB->PSOR = MASK(RED_LED_POS);       //set red led off

	}

	if (green_on) {

			PTB->PCOR = MASK(GREEN_LED_POS);     //set green led on

	}	else {

			PTB->PSOR = MASK(GREEN_LED_POS);    //set green led off

	}

	if (blue_on) {

			PTD->PCOR = MASK(BLUE_LED_POS);       //set blue led on

	}	else {

			PTD->PSOR = MASK(BLUE_LED_POS);     //set blue led off

	}

}
