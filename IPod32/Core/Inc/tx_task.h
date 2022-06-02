/*
 * tx_task.h
 *
 *  Created on: Jun 2, 2022
 *      Author: iandu
 */

#ifndef INC_TX_TASK_H_
#define INC_TX_TASK_H_

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "serial.h"
#include "queue.h"

void transmit_data_task();

#endif /* INC_TX_TASK_H_ */
