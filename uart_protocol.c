/*
 * uart.c
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
#include <ti/drivers/UART.h>
#include "driverlib\sysctl.h"

/* Example/Board Header files */
#include "Board.h"

/* Standard Libraries */
#include <stdlib.h>
#include <string.h>

/* Preprocessor Includes */
#define lightsensor_debug 1

/* Global Variables */

/* Externs */
extern uint16_t snsr1_pd;
extern uint16_t snsr2_pd;
extern uint16_t snsr3_pd;

/* Function prototypes */
void UART_print_int(uint16_t n, UART_Handle uart);
void UART_light_sense(UART_Handle uart);
uint8_t getframe(char * framebuf, UART_Handle handle);
uint32_t str_to_int(char * buf, uint8_t sizebuf);
uint32_t str_checksum(char * buf, uint8_t sizebuf);

/* Constants */
#define timerfreq 10

/* UART comm frame size constants */
#define sizeof_start 	1
#define sizeof_funccode 2
#define sizeof_dlc      2
#define sizeof_checksum 2
#define sizeof_end      2

/* UART comm error codes */
#define error_none     0x0
#define error_nostart  0x1
#define error_checksum 0x2
#define error_nostop   0x3
/*
 *  ======== UART1 ========
 *  UART task for communication over UART1
 */
void UART1(UArg arg0, UArg arg1)
{
    UART_Handle uart;
    UART_Params uartParams;

    /* Frame Character Buffer */
    char framebuffer[100];

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_ON;
    uartParams.baudRate = 115200;
    uart = UART_open(Board_UART1, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    /* Loop forever echoing */
    while (TRUE)
    {
#ifdef lightsensor_debug
    	UART_light_sense(uart);
#endif
    	//uint8_t error = getframe(framebuffer, uart);
    }
}

void UART_print_int(uint16_t n, UART_Handle uart)
{
	if(n>=10)
	{
		UART_print_int(n/10, uart);
		n = n % 10;
	}
	char buf = (char)n + '0';
	UART_write(uart, &buf, 1);
}
uint32_t str_to_int(char * buf, uint8_t sizebuf)
{
	uint8_t n;
	uint32_t result = 0;
	for(n=0; n<sizebuf; n++)
	{
		result = result * 10 + ( buf[n] - '0' );
	}
	return result;
}
uint32_t str_checksum(char * buf, uint8_t sizebuf)
{
	uint8_t n;
	uint32_t result = 0;
	for(n=0; n<sizebuf; n++)
	{
		result += (buf[n] - '0');
	}
	return result;
}
void UART_light_sense(UART_Handle uart)
{
	SysCtlDelay(SysCtlClockGet()/timerfreq);
	UART_write(uart, "Sensor 1 period:", sizeof("Sensor 1 period:"));
	UART_print_int(snsr1_pd, uart);
	UART_write(uart, "\r\n", sizeof("\r\n"));
	UART_write(uart, "Sensor 2 period:", sizeof("Sensor 2 period:"));
	UART_print_int(snsr2_pd, uart);
	UART_write(uart, "\r\n", sizeof("\r\n"));
	UART_write(uart, "Sensor 3 period:", sizeof("Sensor 3 period:"));
	UART_print_int(snsr3_pd, uart);
	UART_write(uart, "\r\n", sizeof("\r\n"));
}
uint8_t getframe(char * framebuf, UART_Handle handle)
{
	// Init temp buffer pointer
	char * framebufptr = framebuf;
	// Init temp buffers
	char checksumbuf[sizeof_checksum];
	char endbuf[sizeof_end];

	/* Read in Start Char */
	UART_read(handle, framebufptr, sizeof_start);
	// Increment buffer pointer
	if(framebufptr[0]!='~') return error_nostart;
	/* Discard start bit by writing FC first to buf */
	UART_read(handle, framebufptr, sizeof_funccode);
	framebufptr++;
	framebufptr++;
	//framebufptr+=sizeof_funccode;
	/* Store data length code */
	UART_read(handle, framebufptr, sizeof_dlc);
	uint32_t dlc = str_to_int(framebufptr, sizeof_dlc);
	framebufptr++;
	framebufptr++;
	//framebufptr += sizeof_dlc;
	/* Store data */
	UART_read(handle, framebufptr, dlc);
	uint32_t data = str_to_int(framebufptr, dlc);
	/* Compute Checksum */
	uint32_t checksum_computed = str_checksum(framebufptr, dlc);
	/* Get Checksum */
	UART_read(handle, checksumbuf, sizeof_checksum);
	uint32_t checksum_frame = str_to_int(checksumbuf, sizeof_checksum);
	/* Check Checksum */
	if(checksum_frame != checksum_computed) return error_checksum;
	/* Check end bits */
	UART_read(handle, endbuf, sizeof_end);
	if(!strcmp(endbuf,"\n\r")) return error_nostop;
	return error_none;
}
