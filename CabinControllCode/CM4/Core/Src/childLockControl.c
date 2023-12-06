/*
 * childLockControl.c
 *
 *  Created on: Oct 17, 2023
 *      Author: Bhargav-4836
 */


/**
 * @file childLockControl.cpp
*/
#include "main.h"
#include "childLockControl.h"
#include "gpio.h"
#include "shared_mem_handler.h"
#include "stdint.h"
#include "stdbool.h"

void checkChildLock();

void checkChildLock()
{
  //todo fix this delay
  static unsigned long	int clTimer;
  clTimer =  HAL_GetTick();

  static bool prevState = false;

  shared_mem_data cl_data = child_lock_disabled;

  if(HAL_GetTick() - clTimer > 100)
  {
    bool childLockStatus = HAL_GPIO_ReadPin(child_lock_GPIO_Port, child_lock_Pin);
    if(prevState!=childLockStatus)
    {
      prevState=childLockStatus;
      if (!childLockStatus)
      {
    	  cl_data = child_lock_enabled;
      }
      else
      {
    	  cl_data = child_lock_disabled;
      }
    }
    send_data_ringbuffer(cl_data);
    clTimer=HAL_GetTick();
  }
}


/// @brief send the current state of child lock
/// @details everytime android reconnects to cabin, send the current state

//void initCL()
//{
//  bool childLockState = HAL_GPIO_ReadPin(child_lock_GPIO_Port, child_lock_Pin);
//  if (!childLockState)
//  {
////    HAL_UART_Transmit(&huart3, (uint8_t *)clEnabled, sizeof(clEnabled), 100);
//    cl_data = child_lock_enabled;
//
//  }
//  else
//  {
////    HAL_UART_Transmit(&huart3, (uint8_t *)clDisabled, sizeof(clDisabled), 100);
//    cl_data = child_lock_disabled;
//
//  }
//}
//
//
