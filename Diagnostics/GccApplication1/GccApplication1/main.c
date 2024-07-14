/*
 * fancontroller.c
 *
 *  Created on: Oct 7, 2022
 *      Author: abdel
 */
#include <avr/io.h>
#include "lcd.h"
#include "motor.h"
#include "lm_sensor.h"
#include "adc.h"
#include "DEM.h"
#include "DCM.h"
#include "gpio.h"
#include "util/delay.h"
#include "stdio.h"



	

int main(void)
{
	Dem_PreInit();
	Dem_Init();
	//Dem_SetOperationCycleState(DEM_POWER, DEM_CYCLE_STATE_START); /** @req DEM194 */

	

	// Example data for InfoType structures
	uint8 rxData[] = { 0x19, 0x02, 0x7f};
	uint8 txData[20]= {}; // Buffer for the transmitted data

	// Initialize PduInfoType structures
	InfoType RxData = {
		.DataPtr = rxData,
		.DataLength = sizeof(rxData)
	};

	InfoType TxData = {
		.DataPtr = txData,
		.DataLength = sizeof(txData)
	};
	TxDataType* txDataPtr = (TxDataType*)TxData.DataPtr;
		
	
	uint8 mytemp;  //variable to store temperature from sensor
	LCD_init(); //LCD init function called
	ADC_ConfigType adcconfig ={INTERNAL_VOLTAGE,F_CPU_8}; //Config given to ADC
	ADC_init(&adcconfig);
	DcMotor_Init(); //DC_MOTOR_intialization
	GPIO_setupPinDirection(PORTD_ID, PIN4_ID, PIN_INPUT);

	int cyclestate = DEM_CYCLE_STATE_END;



	while(1)
	{

		
		//_delay_ms(100);
		mytemp=LM35_getTemperature();//temperature sensed by sensor stored in mytemp
		
		if ((GPIO_readPin(PORTD_ID,PIN4_ID)== 1)&&(cyclestate!=DEM_CYCLE_STATE_START)){
			Dem_SetOperationCycleState(DEM_POWER, DEM_CYCLE_STATE_START);
			cyclestate = DEM_CYCLE_STATE_START;
		}
		else if ((GPIO_readPin(PORTD_ID,PIN4_ID)== 0)&&(cyclestate!=DEM_CYCLE_STATE_END)){
			Dem_SetOperationCycleState(DEM_POWER, DEM_CYCLE_STATE_END);
			cyclestate = DEM_CYCLE_STATE_END;
		}
		
		
		if((mytemp>=10)&&(mytemp<50))
		{
			
			Dem_SetEventStatus(DEM_EVENT_ID_LOW_OIL_PRESSURE, DEM_EVENT_STATUS_PASSED);
			
			/*If temperature is less than 30, Turn off FAN and display its state and temperature on LCD*/

		LCD_moveCursor(0,1);
		LCD_intgerToString(TxData.DataPtr[0]);

		LCD_moveCursor(0,4);
		LCD_intgerToString(TxData.DataPtr[1]);
		
		LCD_moveCursor(0,6);
		LCD_intgerToString(TxData.DataPtr[2]);

		
		LCD_moveCursor(0,9);
		LCD_intgerToString(0);
		
		LCD_moveCursor(0,10);
		LCD_intgerToString(TxData.DataPtr[3]);
		
		LCD_moveCursor(0,11);
		LCD_intgerToString(TxData.DataPtr[4]);
		
		LCD_moveCursor(0,13);
		LCD_intgerToString(TxData.DataPtr[5]);
		
		
		LCD_moveCursor(1,2);
		LCD_intgerToString(TxData.DataPtr[6]);
		
		LCD_moveCursor(1,5);
		LCD_intgerToString(0);

		LCD_moveCursor(1,6);
		LCD_intgerToString(TxData.DataPtr[7]);
		
		LCD_moveCursor(1,7);
		LCD_intgerToString(TxData.DataPtr[8]);
		
		LCD_moveCursor(1,9);
		LCD_intgerToString(TxData.DataPtr[9]);
		
		LCD_moveCursor(1,12);
		LCD_intgerToString(TxData.DataPtr[10]);
			
			
		}
		else
		{
			Dem_SetEventStatus(DEM_EVENT_ID_LOW_OIL_PRESSURE, DEM_EVENT_STATUS_FAILED);
			
			
					LCD_moveCursor(0,1);
					LCD_intgerToString(TxData.DataPtr[0]);

					LCD_moveCursor(0,4);
					LCD_intgerToString(TxData.DataPtr[1]);
					
					LCD_moveCursor(0,6);
					LCD_intgerToString(TxData.DataPtr[2]);

					
					LCD_moveCursor(0,9);
					LCD_intgerToString(0);
					
					LCD_moveCursor(0,10);
					LCD_intgerToString(TxData.DataPtr[3]);
					
					LCD_moveCursor(0,11);
					LCD_intgerToString(TxData.DataPtr[4]);
					
					LCD_moveCursor(0,13);
					LCD_intgerToString(TxData.DataPtr[5]);
					
					
					LCD_moveCursor(1,2);
					LCD_intgerToString(TxData.DataPtr[6]);
					
					LCD_moveCursor(1,5);
					LCD_intgerToString(0);

					LCD_moveCursor(1,6);
					LCD_intgerToString(TxData.DataPtr[7]);
					
					LCD_moveCursor(1,7);
					LCD_intgerToString(TxData.DataPtr[8]);
					
					LCD_moveCursor(1,9);
					LCD_intgerToString(TxData.DataPtr[9]);
					
					LCD_moveCursor(1,12);
					LCD_intgerToString(TxData.DataPtr[10]);
		}
		if(mytemp>=30)
		{
			
			Dem_SetEventStatus(DEM_EVENT_ID_ENGINE_OVERHEAT, DEM_EVENT_STATUS_FAILED);
			
						LCD_moveCursor(0,1);
						LCD_intgerToString(TxData.DataPtr[0]);

						LCD_moveCursor(0,4);
						LCD_intgerToString(TxData.DataPtr[1]);
						
						LCD_moveCursor(0,6);
						LCD_intgerToString(TxData.DataPtr[2]);

						
						LCD_moveCursor(0,9);
						LCD_intgerToString(0);
						
						LCD_moveCursor(0,10);
						LCD_intgerToString(TxData.DataPtr[3]);
						
						LCD_moveCursor(0,11);
						LCD_intgerToString(TxData.DataPtr[4]);
						
						LCD_moveCursor(0,13);
						LCD_intgerToString(TxData.DataPtr[5]);
						
						
						LCD_moveCursor(1,2);
						LCD_intgerToString(TxData.DataPtr[6]);
						
						LCD_moveCursor(1,5);
						LCD_intgerToString(0);

						LCD_moveCursor(1,6);
						LCD_intgerToString(TxData.DataPtr[7]);
						
						LCD_moveCursor(1,7);
						LCD_intgerToString(TxData.DataPtr[8]);
						
						LCD_moveCursor(1,9);
						LCD_intgerToString(TxData.DataPtr[9]);
						
						LCD_moveCursor(1,12);
						LCD_intgerToString(TxData.DataPtr[10]);
		}
		else {
			Dem_SetEventStatus(DEM_EVENT_ID_ENGINE_OVERHEAT, DEM_EVENT_STATUS_PASSED);
			
			
						LCD_moveCursor(0,1);
						LCD_intgerToString(TxData.DataPtr[0]);

						LCD_moveCursor(0,4);
						LCD_intgerToString(TxData.DataPtr[1]);
						
						LCD_moveCursor(0,6);
						LCD_intgerToString(TxData.DataPtr[2]);

						
						LCD_moveCursor(0,9);
						LCD_intgerToString(0);
						
						LCD_moveCursor(0,10);
						LCD_intgerToString(TxData.DataPtr[3]);
						
						LCD_moveCursor(0,11);
						LCD_intgerToString(TxData.DataPtr[4]);
						
						LCD_moveCursor(0,13);
						LCD_intgerToString(TxData.DataPtr[5]);
						
						
						LCD_moveCursor(1,2);
						LCD_intgerToString(TxData.DataPtr[6]);
						
						LCD_moveCursor(1,5);
						LCD_intgerToString(0);

						LCD_moveCursor(1,6);
						LCD_intgerToString(TxData.DataPtr[7]);
						
						LCD_moveCursor(1,7);
						LCD_intgerToString(TxData.DataPtr[8]);
						
						LCD_moveCursor(1,9);
						LCD_intgerToString(TxData.DataPtr[9]);
						
						LCD_moveCursor(1,12);
						LCD_intgerToString(TxData.DataPtr[10]);
		}
			DspUdsReadDtcInformation(&RxData, &TxData);
	}


}
