#ifndef __MAIN_MSP_H__
#define __MAIN_MSP_H__

#include "at32f4xx.h"

void GPIO_TogglePin(GPIO_Type* GPIOx, uint16_t GPIO_Pin);
void Target_ResetInit(void);

#endif


