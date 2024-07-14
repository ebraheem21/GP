/*
 * File Name : adc.h
 * Created on: Oct 4, 2022
 * Author: Abdelrahman
 * Description : ADC header file
 */

#ifndef ADC_H_
#define ADC_H_

#include "std_types.h"


#define ADC_MAXIMUM_VALUE    1023 //adc bits ((2^bits of ADC)-1)
#define ADC_REF_VOLT_VALUE   2.56 // voltage chosen for ADC

//Clocks that can be chosen for ADC
typedef enum
{
	F_CPU_2=1,F_CPU_4,F_CPU_8,F_CPU_16,F_CPU_32,F_CPU_64,F_CPU_128
}ADC_Prescaler;
//Voltages that can be chosen for ADC
typedef enum
{
AREF,AVCC,reserved,INTERNAL_VOLTAGE
}ADC_ReferenceVolatge;

//Structure to set you configuration to the ADC
typedef struct{
 ADC_ReferenceVolatge ref_volt;
 ADC_Prescaler prescaler;
}ADC_ConfigType;



//ADC initial function to set its configuartion
void ADC_init(const ADC_ConfigType * Config_Ptr);

//ADC function to choose the pin to read from
uint16 ADC_readChannel(uint8 ch_num);

#endif /* ADC_H_ */
