/*
 * LCDTask.c
 *
 *  Created on: Jun 2, 2022
 *      Author: abiel
 */


#include "LCDTask/LCDTask.h"

osMessageQueueId_t lcdMsgHandle;
const osMessageQueueAttr_t lcdMessageHandleAttrs = {
		.name = "lcdMessageHandle",
};

void init_lcd_task(){
	//LCD_Init();
	lcdMsgHandle = osMessageQueueNew(5, sizeof(LCDData), NULL);
	set_lcd("Hello", "world!");
}

void lcd_task(){
	init_lcd_task();

	LCDData data;
	memset(data.line1, 0, sizeof(data.line1));
	memset(data.line2, 0, sizeof(data.line2));
	int line1_char = 0;
	int line2_char = 0;
	while(1){
		osStatus_t ret = osMessageQueueGet(lcdMsgHandle, &data, NULL, osWaitForever);
		if (ret == osOK){
			//New message, reset current char
			line1_char = 0;
			line2_char = 0;
		}

		size_t line1_size = strlen(data.line1);
		size_t line2_size = strlen(data.line2);

		//Draw line 1
		//LCD_Set_Cursor(0, 1);
		for(int i = line1_char; i < line1_char + 16; i++){
			//Send chars
			char test = data.line1[i];
			//LCD_Put_Char(data.line1[i]);
		}

		//Draw line 2
		//LCD_Set_cursor(0, 2);

		//Increment char counts
		line1_char++;
		if(line1_char + 16 >= line1_size){
			line1_char = 0;
		}

		line2_char++;
		if(line2_char + 16 >= line2_size){
			line2_char = 0;
		}

		osDelay(100);
	}
}

osStatus_t set_lcd(char *line1, char *line2){
	LCDData data;
	strncpy(data.line1, line1, sizeof(data.line1));
	strncpy(data.line2, line2, sizeof(data.line2));
	return osMessageQueuePut(lcdMsgHandle, &data, NULL, 1000);
}
