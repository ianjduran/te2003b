/*
 * serial.h
 *
 *  Created on: Jun 1, 2022
 *      Author: iandu
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "main.h"

void USER_USART1_Transmit(uint8_t *pData, uint16_t size);
uint8_t USER_USART1_Receive(void);

#endif /* INC_SERIAL_H_ */
