/*
 * LCDTask.h
 *
 *  Created on: Jun 2, 2022
 *      Author: abiel
 */

#ifndef INC_LCDTASK_LCDTASK_H_
#define INC_LCDTASK_LCDTASK_H_

#include "cmsis_os.h"

typedef struct{
	char line1[128];
	char line2[128];
} LCDData;

void init_lcd_task();
void lcd_task();

osStatus_t set_lcd(char *line1, char *line2);

#endif /* INC_LCDTASK_LCDTASK_H_ */
