/*
 * File Name : motor.h
 * Created on: Oct 4, 2022
 * Author: Abdelrahman
 * Description : Motor Driver header file
 */

#ifndef MOTOR_H_
#define MOTOR_H_
#include "std_types.h"

#define MOTOR_IN_PORT_ID PORTB_ID //Motor input port
#define MOTOR_IN1_PIN_ID PIN0_ID //motor first input pin
#define MOTOR_IN2_PIN_ID PIN1_ID //motor second input pin
#define PIN_DIRECTION PIN_OUTPUT

//States for MOTOR
typedef enum {
STOP,CW,ANTI_CW
}DcMotor_State;
//Motor Initialization function to set input pins and port
void DcMotor_Init(void);
//Motor function to set its state and speed
void DcMotor_Rotate(DcMotor_State state,uint8 speed);
#endif /* MOTOR_H_ */
