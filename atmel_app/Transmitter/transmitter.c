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
#define USOFT_IO_TX B, 4
#define USOFT_RXEN 0
#define USOFT_TXEN 1
#define USOFT_tCorrection -16
#include <uart_soft.h> //add ref to Atmel-Library and reload Studio

#include "../parcel.h"

#define REF_AVCC (0<<REFS0) // reference = AVCC
#define REF_INT  (1<<REFS0) // internal reference 1.1 V

#define ADC_VREF_TYPE REF_INT//((1<<REFS0) | (0<<ADLAR))
#define IO_ADC_V 3 //port b.3

#define MIN_VOLTAGE 3.8 //Voltage when we need to switch on the LED
#define Rd1 4700
#define Rd2 1000
#define MIN_VOLTAGE_Rd MIN_VOLTAGE*Rd2/(Rd1+Rd2) //The real voltage in voltage divider
#define MIN_VOLTAGE_ADC MIN_VOLTAGE_Rd*1023 / 1.1 //1023 is max of ADC, 1.1V is the real value of maxADC

#define IO_LED B, 0

void adc_init(void)
{
	ADCSRA = (1<<ADEN)|(0<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // prescaler 8 => 75 kHz
	DIDR0 |= (0<<ADC0D)|(0<<ADC1D)|(0<<ADC2D)|(1<<ADC3D); //disable digital input for selected ADC
}

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
	ADMUX=adc_input | ADC_VREF_TYPE;
	_delay_us(10); // Delay needed for the stabilization of the ADC input voltage
	ADCSRA|=(1<<ADSC); // Start the AD conversion
	while ((ADCSRA & (1<<ADIF))==0); // Wait for the AD conversion to complete
	ADCSRA|=(1<<ADIF);
	return ADCW;
}

const uint8_t adcCount = 5;
int main(void)
{
	adc_init();
	usoft_init();
	io_set(DDR, IO_LED);
	
	while (1)
	{
		for (uint8_t i = 0; i < PARCEL_SIZE; ++i)
		{
			usoft_putCharf(PARCEL_CHAR);
		}
		delay_ms(300);
		
		//voltage indicator
		unsigned int voltage = 0;
		for (uint8_t i = 0; i < adcCount; ++i)
		{
			_delay_us(100);
			voltage += read_adc(IO_ADC_V);
		}
		voltage /= adcCount;

		if (voltage < MIN_VOLTAGE_ADC){
			io_togglePort(IO_LED);
		}
		else {
			io_resetPort(IO_LED);
		}
		
	}
}

