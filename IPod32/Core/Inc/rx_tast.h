/*
 * rx_tast.h
 *
 *  Created on: Jun 1, 2022
 *      Author: iandu
 */

#ifndef INC_RX_TAST_H_
#define INC_RX_TAST_H_

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "serial.h"
#include "queue.h"

void receive_data_task();

#endif /* INC_RX_TAST_H_ */
