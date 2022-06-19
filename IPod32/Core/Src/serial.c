/*
 * serial.c
 *
 *  Created on: Jun 1, 2022
 *      Author: iandu
 */

#include "serial.h"

void USER_USART1_Transmit(uint8_t *pData, uint16_t size) {
	for (int i = 0; i < size; i++) {
		while ((USART1->SR & USART_SR_TXE) == 0) {

		}
		USART1->DR = *pData++;
	}
}

uint8_t USER_USART1_Receive(void) {
	while ((USART1->SR & USART_SR_RXNE) == 0) {

	}
	return USART1->DR;
}
