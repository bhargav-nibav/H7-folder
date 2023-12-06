/*
 * callReg.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_CALLREG_H_
#define INC_CALLREG_H_





#endif /* INC_CALLREG_H_ */


#ifndef __CALL_REG_H__
#define __CALL_REG_H__
#include <stdint.h>

void registerCabinCall();
void processFactorySettings(uint8_t _widgetNumber, uint8_t _rbyte);
void checkSirenStatus();
void heartBeatStatus();
void getVersionData();
void checkShaftDataReq();
#endif

