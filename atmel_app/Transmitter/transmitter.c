/*
* Transmitter.c
*
* Created: 05.06.2019 17:18:56
* Author : YegorVM
*/
#define F_CPU 4800000UL
#include <avr/io.h>
#include <extensions.h> //add ref to Atmel-Library and reload Studio

#define USOFT_BAUD 1200
#define USOFT_IO_TX B, 4
#define USOFT_RXEN 0
#define USOFT_TXEN 1
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

int main(void)
{
	usoft_init();
	while (1)
	{
	}
}
