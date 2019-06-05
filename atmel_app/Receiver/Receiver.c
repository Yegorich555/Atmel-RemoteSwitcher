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

#if DEBUG
#define USOFT_TXEN 1
#define USOFT_IO_TX B, 1
#else
#define USOFT_TXEN 1
#endif
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

int main(void)
{
	usoft_init();
	while (1)
	{
	}
}

