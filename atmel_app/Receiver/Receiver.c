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

#define PERCENT_Quality 50
#define QUALITY_Set (sizeof parcel * PERCENT_Quality / 100)
volatile bool isNeedTrigger;

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

int prevIndex = -1;
int cntGood = 0;

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

USOFT_ISR_newByte(b)
{
	if (prevIndex == -1)
	{
		while (prevIndex < sizeof parcel - QUALITY_Set)
		{
			if (b == parcel[++prevIndex])
			{
				cntGood = 1;
				return;
			}
		}
		prevIndex = -1;
	}
	else if (b == parcel[++prevIndex])
	{
		isNeedTrigger = ++cntGood >= QUALITY_Set;
	}

	if (isNeedTrigger || prevIndex >= sizeof parcel - 1)
	{
		cntGood = 0;
		prevIndex = -1;
	}
}
