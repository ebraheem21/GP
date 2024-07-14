/*
 * lm_sensor.h
 *
 *  Created on: Oct 8, 2022
 *      Author: abdel
 */

#ifndef LM_SENSOR_H_
#define LM_SENSOR_H_


#include "std_types.h"


#define SENSOR_CHANNEL_ID         2
#define SENSOR_MAX_VOLT_VALUE     1.5
#define SENSOR_MAX_TEMPERATURE    150


uint8 LM35_getTemperature(void);

#endif /* LM_SENSOR_H_ */
