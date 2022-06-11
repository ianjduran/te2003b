/*
 * RFIDTask.c
 *
 *  Created on: Jun 8, 2022
 *      Author: abiel
 */

#include "RFIDTask.h"
#include "cmsis_os2.h"
#include "MFRC522.h"

void rfid_task(void *params){
	MFRC522_Init();
	uint8_t status = Read_MFRC522(VersionReg);


	uint8_t buf[MAX_LEN + 1];
	for(;;){
		uint8_t ret = MFRC522_Request(PICC_REQIDL, buf);
		if(ret == 0){
			osDelay(10);
		}
		osDelay(100);
	}
}
