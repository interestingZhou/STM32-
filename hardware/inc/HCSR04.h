#include "stdint.h"
#ifndef __HCSR04_H
#define __HCSR04_H

void HC_SR04_Init(void);
uint16_t HCSR04_Read_Data(uint16_t *Distance_mm);
float sonar(void);

#endif
