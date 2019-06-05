/*
* Receiver.c
*
* Created: 05.06.2019 17:19:48
* Author : YegorVM
*/
#define F_CPU 4800000UL
#include <avr/io.h>
#include <extensions.h> //add ref to Atmel-Library and reload Studio

#define IO_OutSwitch B, 3
#define DELAY_between 3000 //delay between switches ms

#define USOFT_BAUD 1200
#define USOFT_IO_RX B, 4
#define USOFT_RXEN 1
#define USOFT_BUFFER_EN 0

#if DEBUG
#define USOFT_TXEN 1
#define USOFT_IO_TX B, 1
#else
#define USOFT_TXEN 0
#endif
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

#include "../parcel.h"
const char parcel[] = STR_PARCEL;

unsigned char myByte;
bool isNeedTrigger;

int main(void)
{
	usoft_init();
	io_set(DDR, IO_OutSwitch);
	
	while (1)
	{
		usoft_listen();
		
		if (isNeedTrigger)
		{
			io_togglePort(IO_OutSwitch);
			isNeedTrigger = false;
			delay_ms(DELAY_between);
		}
	}
}

USOFT_ISR_newByte(b)
{
	int size = sizeof parcel;
	myByte = b;
	//todo analyze of the parcel
}
