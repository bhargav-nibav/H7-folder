/*
 * lightFanControl.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */



/**
 * @file lightFanControl.cpp
*/

#include "lightFanControl.h"
#include "debug.h"


static void disableLightFan();
static void checkFanLightPWM();

bool setTimeFlag = true;
bool firstTimeRest = true;
unsigned long initTime = 0;
unsigned long fanTimeOut = 0;
bool prevFanLightState = false;

bool pwmTimerFlag;
bool enablePWMTimer = false;

int pwmVal1 = 20;
int pwmVal2 = 20;


char buff[60];
void initPWM()
{
  setDeviceIntensity(LIGHT_OUTPUT_GPIO, pwmVal1);
  setDeviceIntensity(FAN_OUTPUT_GPIO, pwmVal2);
}

/// @brief Function to set pwm value based on input from android
/// @param deviceID either fan or light gpio
/// @param pwmValue value from 0 to 100
void setDeviceIntensity(int deviceID, int pwmValue)
{
  pwmValue = 100 - pwmValue;
//  analogWrite(deviceID, pwmValue * 2.55);
}

void storeDeviceIntensity(int deviceID, int pwmValue)
{
  if(deviceID == LIGHT_OUTPUT_GPIO)
  {
    pwmVal1 = pwmValue;
  }
  else if(deviceID == FAN_OUTPUT_GPIO)
  {
    pwmVal2 = pwmValue;
  }
    else if(deviceID == FAN_OUTPUT_GPIO)
  {
    pwmVal2 = pwmValue;
  }
  else
  {
    HAL_UART_Transmit(&huart3, (uint8_t *)"Wrong device ID", sizeof("Wrong device ID"), 100);
  }
}

void processFanLightControl()
{
  checkFanLightPWM();
  disableLightFan();
}

void disableLightFan()
{
  if (enablePWMTimer)
  {
    firstTimeRest = false;
    setTimeFlag = false;
  }
  enablePWMTimer = false;

  if (!setTimeFlag && !firstTimeRest)
  {
    // enable fan light
    controlPWMState(true);
    initTime = HAL_GetTick();
    fanTimeOut = (initTime + 30 * 1000);
    setTimeFlag = true;
  }
  else if (!setTimeFlag && firstTimeRest)
  {
//	  HAL_UART_Transmit(&huart3, (uint8_t *)"One time ran", sizeof("One time ran"), 100);
    controlPWMState(false);
    initTime = HAL_GetTick();
  }
  else if (setTimeFlag && !firstTimeRest)
  {
//	  HAL_UART_Transmit(&huart3, (uint8_t *)"First time set", sizeof("First time set"), 100);
    if (initTime < fanTimeOut)
    {
      controlPWMState(true);
      //      HAL_UART_Transmit(&huart3, (uint8_t *)"not timeout", sizeof("not timeout"), 100);
      initTime = HAL_GetTick();
    }
    else if ((initTime >= fanTimeOut))
    {
      controlPWMState(false);
      setTimeFlag = true;
      initTime = HAL_GetTick();
      firstTimeRest = true;
      HAL_UART_Transmit(&huart3, (uint8_t *)"flag cleared", sizeof("flag cleared"), 100);
    }
  }
  else if (setTimeFlag && firstTimeRest)
  {
    // Serial.println("Wrong zone");
    // controlPWMState(false);
    initTime = HAL_GetTick();
  }
}

void checkFanLightPWM()
{
  bool state = pwmTimerFlag;

  if (prevFanLightState != state)
  {
    if (state)
    {
      // run the fan and light at last saved value
      int len = snprintf(buff, sizeof(buff), ("Value of pwmVal1 = %d pwmVal2 = %d"), (pwmVal1, pwmVal2));
      HAL_UART_Transmit(&huart3, (uint8_t *) buff, len, 100);
      setDeviceIntensity(LIGHT_OUTPUT_GPIO, pwmVal1);
      setDeviceIntensity(FAN_OUTPUT_GPIO, pwmVal2);
    }
    else
    {
      //turn off fan and light
      setDeviceIntensity(LIGHT_OUTPUT_GPIO, 0);
      setDeviceIntensity(FAN_OUTPUT_GPIO, 0);
    }
  }
  prevFanLightState = state;
}

void controlPWMState(bool val)
{
  pwmTimerFlag = val;
}

void resetTimer()
{
  setTimeFlag = true;
  firstTimeRest = true;
}
