/*
 * VoltageControl.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_VOLTAGECONTROL_H_
#define INC_VOLTAGECONTROL_H_





#endif /* INC_VOLTAGECONTROL_H_ */


#include <usart.h>

void initVoltage();
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void vol_measurement();
void displayPercentage();
int processPercentage(int _bat_percentage);
void toggleCCFail(bool state);
int movingAverage(int _input_percentage);
void lowPowerModeDisplay(uint8_t State);
void initCL();
void checkbatteryStatus();
void displayIcon();
void setBatteryTimerInit();
