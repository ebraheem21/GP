/*
 * File Name : motor.c
 * Created on: Oct 4, 2022
 * Author: Abdelrahman
 * Description : Motor Driver Source file
 */




#include "motor.h"
#include <avr/io.h>
#include "common_macros.h"
#include "gpio.h"
#include "pwm.h"



void DcMotor_Init(void)
{
/*Motor input port and pins set*/
GPIO_setupPinDirection(MOTOR_IN_PORT_ID,MOTOR_IN1_PIN_ID,PIN_DIRECTION);
GPIO_setupPinDirection(MOTOR_IN_PORT_ID,MOTOR_IN2_PIN_ID,PIN_DIRECTION);
GPIO_writePin(MOTOR_IN_PORT_ID,MOTOR_IN1_PIN_ID ,LOGIC_LOW);
GPIO_writePin(MOTOR_IN_PORT_ID,MOTOR_IN2_PIN_ID ,LOGIC_LOW);

}

void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{

	//Motor state (stop,CW,ACW)
	GPIO_writePort(MOTOR_IN_PORT_ID, state);
	/*PWM_ConfigType pwmconfig ={F_CPU_CLOCK_8,CLEAR_OC0_ON_COMPARE};
	 PWM_Timer0_Start(speed,&pwmconfig);
	*/
	//Motor Speed passed to Timer with PWM mode
	PWM_Timer0_Start(speed);
}
