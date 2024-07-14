/*
 * File Name : pwm.c
 * Created on: Oct 4, 2022
 * Author: Abdelrahman
 * Description : PWM Driver Source file
 */
#include <avr/io.h>
#include "common_macros.h"
#include "pwm.h"
#include "gpio.h"
void PWM_Timer0_Start(uint8 duty_cycle)
{


	TCNT0 = 0; // Set Timer Initial Value to 0

	OCR0  = duty_cycle; //Set Compare value
	GPIO_setupPinDirection(PWM_OUTPUT_PORT,PWM_OUTPUT_PIN,PIN_OUTPUT);

		/* Configure timer control register
		 * 1. Fast PWM mode FOC0=0
		 * 2. Fast PWM Mode WGM01=1 & WGM00=1
		 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
		 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
		 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);


}
