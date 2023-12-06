/*
 * lightFanControl.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */


#ifndef __LIGHT_FAN_CONTROL_H__
#define __LIGHT_FAN_CONTROL_H__

#include "usart.h"
#include "common.h"
#include "stdbool.h"

#define LIGHT_OUTPUT_GPIO 36
#define FAN_OUTPUT_GPIO 35
#define NIBAV_OUTPUT_GPIO 35

void setDeviceIntensity(int deviceID, int pwmValue);
void processFanLightControl();
void initPWM();

void storeDeviceIntensity(int deviceID, int pwmValue);

void controlPWMState(bool val);
void resetTimer();


#endif /* INC_LIGHTFANCONTROL_H_ */
