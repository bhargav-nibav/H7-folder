/*
 * childLockControl.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */


/**
 * @file childLockControl.cpp
*/
#include "childLockControl.h"
#include "main.h"
#include "gpio.h"
#include "messageQueue.h"

void initCL();

const char clEnable[] = "Child lock enabled";
const char clDisable[] = "Child lock disabled";

/// @brief Set the GPIO pin as input for detecting child lock button press
void sendCLStatus(bool clState);


/// @brief Continously monitor the child lock button status
/// @details If the button is pressed or released, send the relevant message to display as well as shaft
/// @author Balaji M
void checkChildLock()
{
  //todo fix this delay
  static long int clTimer;
  clTimer = HAL_GetTick();
//  static bool checkTimer = false;
  static bool prevState = false;
  if(HAL_GetTick() - clTimer > 100)
  {
    bool childLockStatus = HAL_GPIO_ReadPin(child_lock_GPIO_Port, child_lock_Pin);
    if(prevState!=childLockStatus)
    {
      prevState=childLockStatus;
      if (!childLockStatus)
      {
//        Serial.println("CL button pressed");
        HAL_UART_Transmit(&huart3, (uint8_t *)clEnable, sizeof(clEnable), 100);
        sendCLStatus(true);
      }
      else
      {
//        Serial.println("CL button released");
        HAL_UART_Transmit(&huart3, (uint8_t *)clDisable, sizeof(clDisable), 100);
        sendCLStatus(false);
      }
    }
    clTimer=HAL_GetTick();
  }
}

/// @brief Send child lock disable message to android display
/// @details 0x0B disable Child Lock
void disableChildLock()
{
  androidDataframe(CHILDLOCK_STATUS, CHILD_LOCK_DISABLED);
}

/// @brief Send child lock enable message to android display
/// @details 0x0C disable Child Lock
void enableChildLock()
{
androidDataframe(CHILDLOCK_STATUS, CHILD_LOCK_ENABLED);
}

/// @brief send the current state of child lock
/// @details everytime android reconnects to cabin, send the current state
void initCL()
{
  bool childLockState = HAL_GPIO_ReadPin(child_lock_GPIO_Port, child_lock_Pin);
  if (!childLockState)
  {
    enableChildLock();
//    Serial.println("Child lock enabled");.
    HAL_UART_Transmit(&huart3, (uint8_t *)clEnable, sizeof(clEnable), 100);
  }
  else
  {
//    Serial.println("Child lock disabled");
	  HAL_UART_Transmit(&huart3, (uint8_t *)clDisable, sizeof(clDisable), 100);
    disableChildLock();
  }
}

void sendCLStatus(bool clState)
{
  if(clState)
  {
    enableChildLock();
//    Serial.println("Child lock enabled");
    HAL_UART_Transmit(&huart3, (uint8_t *)clEnable, sizeof(clEnable), 100);
    sendLiftChildlock(true);
  }
  else
  {
    sendLiftChildlock(false);
//    Serial.println("Child lock disabled");
    HAL_UART_Transmit(&huart3, (uint8_t *)clDisable, sizeof(clDisable), 100);
    disableChildLock();
  }
}
