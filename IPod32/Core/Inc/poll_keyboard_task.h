/*
 * poll_keyboard_task.h
 *
 *  Created on: May 30, 2022
 *      Author: Ian Duran
 */

#ifndef INC_POLL_KEYBOARD_TASK_H_
#define INC_POLL_KEYBOARD_TASK_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "main.h"

void poll_keyboard_button_task();

#endif /* INC_POLL_KEYBOARD_TASK_H_ */
