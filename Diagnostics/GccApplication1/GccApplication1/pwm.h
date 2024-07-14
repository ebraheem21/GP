/*
 * File Name : pwm.h
 * Created on: Oct 4, 2022
 * Author: Abdelrahman
 * Description : PWM Driver header file
 */

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

#define PWM_OUTPUT_PIN  PIN3_ID // PWM output pin
#define PWM_OUTPUT_PORT PORTB_ID // PWM ouput port


/*typedef enum
{
NO_CLOCK,F_CPU_CLOCK,F_CPU_CLOCK_8,F_CPU_CLOCK_64,F_CPU_CLOCK_256,F_CPU_CLOCK_1024,EXTERNAL_CLOCK_FALLING,EXTERNAL_CLOCK_RISING
}TIMER_PRESCALER;

typedef enum
{
OC0_DISCONNECTED,RESERVED,CLEAR_OC0_ON_COMPARE,SET_OC0_ON_COMPARE
}COMPARE_FAST_PWM_MODE;

typedef struct{
 TIMER_PRESCALER timerprescaler;
 COMPARE_FAST_PWM_MODE pwm_compare;
}PWM_ConfigType;
*/

void PWM_Timer0_Start(uint8 duty_cycle); // Function to start your timer given duty cycled required
//void PWM_Timer0_Start(uint8 duty_cycle,const PWM_ConfigType * Config_Ptr);
#endif /* PWM_H_ */
