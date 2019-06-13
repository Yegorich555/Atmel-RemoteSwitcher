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
#define USOFT_TXEN 0
#define USOFT_IO_TX IO_DEBUG_TX
#else
#define USOFT_TXEN 0
#endif
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

#include "../parcel.h"

#define PERCENT_Quality 50
#define QUALITY_Set (STR_PARCEL_SIZE * PERCENT_Quality / 100)
volatile bool isNeedTrigger;

//Test 'Gol26735\n' OK
//Test 'ol26735\n' OK
//Test 'Gol26735' OK
//Test 'Gol2' OK
//Test 'l267' OK
//Test 'G_l26735\n' OK
//Test 'bad_Gol26735\n' OK
//
//Test 'GGol26735\n' OK
//Test 'GolGol26735\n' OK
//Test 'ol2Gol26' failed
//Test 'ol_6Gol26' failed
//Test '6735\nGol26735\n' OK
//Test '735\nGol26735\n' OK
//
//Test 'GsomeText' need wrong: OK
//Test 'long parcel_Gol26735' OK
int main(void)
{
	#define TEST_PARCEL "ol2Gol26"  //"ol2Gol26"
	for (uint8_t i=0;i< sizeof TEST_PARCEL;++i)
	usoft_newByte(TEST_PARCEL[i]);
	
	if (isNeedTrigger)
	return 1;
	else
	return 0;
}

int main2(void)
{
	return 1;
	usoft_init();
	io_set(DDR, IO_OutSwitch);
	
	//delay_ms(200);
	//usoft_putStringf(buf);
	while (1)
	{
		if (isNeedTrigger)
		{
			usoft_putStringf("ok\n");
			
			#ifndef DEBUG
			io_togglePort(IO_OutSwitch);
			delay_ms(DELAY_between);
			#endif
			
			isNeedTrigger = false;
		}
		else
		{
			usoft_listen();
		}
	}
}

volatile uint8_t bufStartIndex;
volatile uint8_t bufLastIndex;
volatile uint8_t parcelIndex = 0;
unsigned char buf[STR_PARCEL_SIZE];
void checkBuffer()
{
	if (bufLastIndex - bufStartIndex < QUALITY_Set - 1)
	{
		return;
	}
	
	uint8_t i = bufStartIndex;
	parcelIndex = 0;
	uint8_t cntGood = 0;
	
	void searchStart()
	{
		for (; i < bufLastIndex - (STR_PARCEL_SIZE - QUALITY_Set - 2); ++i, ++bufStartIndex)
		{
			for (; parcelIndex < (STR_PARCEL_SIZE - QUALITY_Set + 1); ++parcelIndex)
			{
				if (buf[i%STR_PARCEL_SIZE] == STR_PARCEL[parcelIndex])
				{
					cntGood = 1;
					return;
				}
			}
		}
	}
	searchStart();
	if (cntGood == 0)
	{
		return;
	}
	//todo check i, parceIndex
	while(++i <= bufLastIndex)
	{
		if (buf[i%STR_PARCEL_SIZE] == STR_PARCEL[++parcelIndex])
		{
			++cntGood;
			isNeedTrigger = cntGood >= QUALITY_Set;
		}
		
		if (isNeedTrigger)
		{
			bufStartIndex = 0;
			bufLastIndex = 0;
			break;
		}
		else if (QUALITY_Set - cntGood > STR_PARCEL_SIZE - parcelIndex - 1) // parcelIndex >= (STR_PARCEL_SIZE - 1)) 
		//#define TEST_PARCEL "ol_6Gol26"
		//#define TEST_PARCEL "012345678"
		{
			++bufStartIndex;
			break;
		}
	}
}

USOFT_ISR_newByte(b)
{
	buf[bufLastIndex%STR_PARCEL_SIZE] = b;
	
	checkBuffer();
	
	++bufLastIndex;
	if (bufLastIndex > 254)
	{
		bufLastIndex = 0;
	}
}