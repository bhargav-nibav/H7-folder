/*
 * common.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_



#include "stdlib.h"
#include "stdint.h"
#include "string.h"

#define    SET_BIT(reg, n)     (reg |= (1 << n))
#define    CLR_BIT(reg, n)     (reg &= ~(1 << n))
#define    CHECK_BIT(reg, n)   (reg & (1 << n))

#define LENGTH_OF_ARRAY(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define ONE_SECOND 1000

#define CABIN_MINOR 0x01
#define CABIN_MAJOR 0x01

#define SHAFT_MINOR 0x00
#define SHAFT_MAJOR 0x01

uint32_t tickDiff(uint32_t startTime, uint32_t currentTime);




#endif /* INC_COMMON_H_ */
