/*
 * RFIDTask.h
 *
 *  Created on: Jun 8, 2022
 *      Author: abiel
 */

#ifndef SRC_RFIDTASK_H_
#define SRC_RFIDTASK_H_

#include "main.h"

int RC522_checkCard(uint8_t *id);
void rfid_task(void *params);


#endif /* SRC_RFIDTASK_H_ */
