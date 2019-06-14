/*
* Receiver.c
*
* Created: 05.06.2019 17:19:48
* Author : YegorVM
*/
#define F_CPU 9600000UL
#include <avr/io.h>
#include <extensions.h> //add ref to Atmel-Library and reload Studio

#define IO_OutSwitch B, 3
#define DELAY_between 3000 //delay between switches ms

#define USOFT_BAUD 1200
#if DEBUG
#define USOFT_IO_RX IO_DEBUG_RX //B, 4
#else
#define USOFT_IO_RX B, 4
#endif

#define USOFT_RXEN 1
#define USOFT_BUFFER_EN 0
#define USOFT_NewByteEvent_EN 1

#if DEBUG
#define USOFT_TXEN 1
#define USOFT_IO_TX IO_DEBUG_TX
#else
#define USOFT_TXEN 0
#endif
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

#include "../parcel.h"

#define QUALITY_Set 3 //symbols in one order
volatile bool isNeedTrigger;

//Test 'G_G_G_G_G' OK
//Test 'GGG' OK
//Test '__GGG' OK
//int main(void)
//{
//#define TEST_PARCEL "__GGG"
//for (uint8_t i=0;i< sizeof TEST_PARCEL;++i)
//usoft_newByte(TEST_PARCEL[i]);
//
//if (isNeedTrigger)
//return 1;
//else
//return 0;
//}

int main(void)
{
	usoft_init();
	io_set(DDR, IO_OutSwitch);
	
	while (1)
	{
		usoft_listen();
		
		if (isNeedTrigger)
		{
			#if DEBUG
			usoft_putStringf("ok\n");
			#else
			io_togglePort(IO_OutSwitch);
			#endif
			
			for (uint16_t i=0; i < DELAY_between*10*0.7; ++i) //*10 because 100us interval; *.8 because the real time is increased by nested operations and interrupts
			{
				usoft_listen();
				if (isNeedTrigger)
				{
					isNeedTrigger = false;
					i = 0;
				}
				_delay_us(100);
			}
			#if DEBUG
			usoft_putStringf("go\n");
			#endif
		}
	}
}

volatile uint8_t cntGood;
volatile uint8_t cntForBad;
USOFT_ISR_newByte(b)
{
	if (b == PARCEL_CHAR)
	{
		++cntGood;
		cntForBad = 0;
		if (cntGood >= QUALITY_Set)
		{
			isNeedTrigger = true;
			cntGood = 0;
		}
	}
	else if (cntGood && ++cntForBad == 2)
	{
		--cntGood;
		cntForBad = 0;
	}
}