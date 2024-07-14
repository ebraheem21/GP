/*
 * File Name : adcc
 * Created on: Oct 4, 2022
 * Author: Abdelrahman
 * Description : ADC Driver Source file
 */

#include "adc.h"
#include "lcd.h"

#include <avr/io.h>
#include "common_macros.h"


void ADC_init(const ADC_ConfigType * Config_Ptr)
{
	ADMUX=(ADMUX&0x3F)|((Config_Ptr->ref_volt)<<6); //Set voltage bits in ADMUX register
	SET_BIT(ADCSRA,ADEN); //enable ADC
	CLEAR_BIT(ADCSRA,ADIF);//disable ADC interrupt
	ADCSRA = (ADCSRA & 0xF8) | (Config_Ptr->prescaler);//set ADC Clock prescaler for adc

}



uint16 ADC_readChannel(uint8 ch_num)
{
	ch_num &= 0x07; // Input channel number must be from 0 to 7
	ADMUX &= 0xE0; //Clear first 5 bits in the ADMUX (MUX4:0 bits)
	ADMUX = ADMUX | ch_num; // Choose the correct channel by setting the channel number in MUX4:0 bits

	SET_BIT(ADCSRA,ADSC);//set ADSC bit to start conversion
	while(BIT_IS_CLEAR(ADCSRA,ADIF));//Polling till ADIF is set as an indicator to the end of conversion
	SET_BIT(ADCSRA,ADIF);//Clear ADIF by setting '1' to it
	return ADC; //return digital value from adc register


}
