/*
 * Bootloader_Config.h
 *
 *  Created on: Feb 18, 2024
 *      Author: Amr
 */

#ifndef INC_BOOTLOADER_CONFIG_H_
#define INC_BOOTLOADER_CONFIG_H_

/* ----------------- Macro Declarations -----------------*/

#define BL_HOST_COMMUNICATION_UART_SEND      &huart2
#define BL_HOST_COMMUNICATION_UART_RECEIVE   &huart1
#define BL_DEBUG                             &huart6

#define DEBUG_INFO_ENABLE                    1
#define DEBUG_INFO_DISABLE                   0

#define BL_DEBUG_ENABLE                      DEBUG_INFO_DISABLE

#define BL_UART_DEBUG_MESSAGE                0x01
#define BL_USB_DEBUG_MESSAGE                 0x02

#define BL_DEBUG_METHOD                      BL_UART_DEBUG_MESSAGE



#endif /* INC_BOOTLOADER_CONFIG_H_ */
