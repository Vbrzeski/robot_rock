/*
 * light_sensor.c
 *
 *  Created on: Nov 3, 2014
 *      Author: Victor Brzeski
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Pin Macro Header Files */
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

/* Board Header file */
#include "Board.h"

/* Global variables for light sensor */
uint16_t snsr1_pd;
uint16_t snsr2_pd;
uint16_t snsr3_pd;

void light_sensor_IRQ(void)
{
	static uint8_t state = 0;
	static uint16_t cycles_snsr1 = 0;
	static uint16_t cycles_snsr2 = 0;
	static uint16_t cycles_snsr3 = 0;
	static int32_t snsr_val = 0;
	static uint16_t count = 0;
	switch(state)
	{
	case 0:
		/* Set pins to output */
		GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, lightsnsr);
		/* Set Pins High */
		GPIOPinWrite(GPIO_PORTA_BASE, lightsnsr, lightsnsr);
		/* Change States */
		state++;
	break;
	case 1:
		/* Set pins to input */
		GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, lightsnsr);
		/*Increment Counters */
		cycles_snsr1++;
		cycles_snsr2++;
		cycles_snsr3++;
		/* Change States */
		state++;
	break;
	case 2:
		/* Check if pins are logic low */
		snsr_val = GPIOPinRead(GPIO_PORTA_BASE, lightsnsr);
		/* Test Counter */
		count++;
		if((snsr_val & lightsnsr1) == lightsnsr1) cycles_snsr1++;
		if((snsr_val & lightsnsr2) == lightsnsr2) cycles_snsr2++;
		if((snsr_val & lightsnsr3) == lightsnsr3) cycles_snsr3++;
		/* Are all sensor values zero ? */
		if(snsr_val == 0)
		{
			/* Measure time */
			snsr1_pd = 10 * cycles_snsr1;
			snsr2_pd = 10 * cycles_snsr2;
			snsr3_pd = 10 * cycles_snsr3;
			/* Reset state to zero */
			state=0;
			/* Reset timer cycles to zero */
			cycles_snsr1 = 0;
			cycles_snsr2 = 0;
			cycles_snsr3 = 0;
		}
	break;
	}
}



