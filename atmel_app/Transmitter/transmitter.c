/*
* Transmitter.c
*
* Created: 05.06.2019 17:18:56
* Author : YegorVM
*/
#define F_CPU 600000UL //fuses for 4.8MHz CKDIV8==1
#include <avr/io.h>
#include <extensions.h> //add ref to Atmel-Library and reload Studio

#define USOFT_BAUD 1200
#define USOFT_IO_TX B, 3
#define USOFT_RXEN 0
#define USOFT_TXEN 1
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

#include "../parcel.h"

int main(void)
{
	usoft_init();
	while (1)
	{
		for (uint8_t i = 0; i < PARCEL_SIZE; ++i)
		{
			usoft_putCharf(PARCEL_CHAR);
		}
		delay_ms(10);
	}
}

