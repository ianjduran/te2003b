/*
 * RFIDTask.c
 *
 *  Created on: Jun 8, 2022
 *      Author: abiel
 */

#include "RFIDTask.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "MFRC522.h"
#include "LCDTask/LCDTask.h"
#include "queue.h"

extern osMessageQueueId_t inputQueueHandle;

int RC522_checkCard(uint8_t *id) {
	int status = 0;
	//Find cards, return card type
	status = MFRC522_Request(PICC_REQIDL, id);
	if (status == MI_OK) {
		//Card detected
		//Anti-collision, return card serial number 4 bytes
		status = MFRC522_Anticoll(id);

	}
	MFRC522_Halt();      //Command card into hibernation

	return status;
}

void rfid_task(void *params) {
	MFRC522_Init();
//	uint8_t status = Read_MFRC522(VersionReg);
	uint8_t rfid_id[4];
	uint8_t str1[128];
	GPIOC->BSRR = GPIO_BSRR_BR13;
//	uint8_t buf[MAX_LEN + 1];
	for (;;) {
		char res[2];
		uint8_t id;

		if (RC522_checkCard(rfid_id) == 0) {
//			GPIOC->BSRR = GPIO_BSRR_BS13;
			sprintf(str1, "Card:%x,%x,%x,%x", rfid_id[0], rfid_id[1],
					rfid_id[2], rfid_id[3]);
			if (rfid_id[0] == 0x1a && rfid_id[1] == 0xe8 && rfid_id[2] == 0xc2
					&& rfid_id[3] == 0x82) {
				id = '1'; // TAG 1
			} else if (rfid_id[0] == 0x9c && rfid_id[1] == 0xe8
					&& rfid_id[2] == 0x76 && rfid_id[3] == 0x6e) { // TAG 2
				id = '2';
			} else if (rfid_id[0] == 0x83 && rfid_id[1] == 0xf0
					&& rfid_id[2] == 0x2e && rfid_id[3] == 0xd) { // TAG 3
				id = '3';
			} else {
				id = '0';
			}
//			set_lcd(str1, "");
			snprintf(res, sizeof(res), "%c", id);
			osMessageQueuePut(inputQueueHandle, &res, 0U, 100);
			osDelay(1);
		}
	}
}

