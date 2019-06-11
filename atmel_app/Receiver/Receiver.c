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
#ifndef DEBUG
#define DELAY_between 3000 //delay between switches ms
#endif

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

#define PERCENT_Quality 50
#define QUALITY_Set (STR_PARCEL_SIZE * PERCENT_Quality / 100)
volatile bool isNeedTrigger;

//test
//int main(void)
//{
	//for (int i=0;i<STR_PARCEL_SIZE;++i)
	 //usoft_newByte(STR_PARCEL[i]);
//}

int main(void)
{
	MCUCR &= ~(1<<ISC01) | ~(0<<ISC00);	// Trigger INT0 on rising edge
	PCMSK |= (1<<PCINT4);   // pin change mask: listen to portb
	GIMSK |= (1<<PCIE); // enable PCINT interrupt
	usoft_init();
	
	io_set(DDR, IO_OutSwitch);
	//delay_ms(200);
	//usoft_putStringf(buf);
	while (1)
	{
		//usoft_listen();
		if (isNeedTrigger)
		{
			usoft_putStringf("ok\n");
			
			#ifndef DEBUG
			io_togglePort(IO_OutSwitch);
			delay_ms(DELAY_between);
			#endif
			
			isNeedTrigger = false;
		}
	}
}

ISR(PCINT0_vect)
{
	usoft_listen();
}

//Tested parcel= 'Gol26375\n'
//qualitySet=4
//Test 'Gol26375\n' OK
//Test 'ol26375\n' OK
//Test 'Gol26375' OK
//Test 'Gol2' OK
//Test 'l263' OK
//Test 'G_l26375\n' OK
//Test 'bad_Gol26375\n' OK

//Test 'GGol26375\n' failed
//Test 'lGol26375\n' failed
//Test 'GolGol26375\n' failed
//Test 'ol2Gol2' failed
//Test 'ol_6Gol26' failed

//Test 'GsomeText' need wrong: OK

int8_t prevIndex = -1;
uint8_t cntGood = 0;
USOFT_ISR_newByte(b)
{
	if (isNeedTrigger)
	{
		return;
	}
	if (prevIndex == -1)
	{
		while (prevIndex < (int8_t)(STR_PARCEL_SIZE - QUALITY_Set))
		{
			if (b == STR_PARCEL[++prevIndex])
			{
				cntGood = 1;
				return;
			}
		}
		prevIndex = -1;
	}
	else if (b == STR_PARCEL[++prevIndex])
	{
		isNeedTrigger = ++cntGood >= QUALITY_Set;
	}

	if (isNeedTrigger || prevIndex >= (int8_t)STR_PARCEL_SIZE - 1)
	{
		cntGood = 0;
		prevIndex = -1;
	}
}
