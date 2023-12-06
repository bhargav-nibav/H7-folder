/*
 * debug.h
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_



#include "common.h"

#include <usart.h>

#define DEBUG_P 0

#if (DEBUG_P)
/*********************************************************************************************/

#define DEBUG(var, expval, msg) \
  {                             \
    if (var != expval)          \
    {                           \
      var = expval;
      HAL_UART_Transmit(&huart3, (uint8_t *)msg, sizeof(msg)), 100); \
    }                           \
  }

#else
//#define DEBUG(var, expval, msg)
#endif

#define DEBUG_ctrlx 1


#if (DEBUG_ctrlx)
/*********************************************************************************************/

#define DEBUG_ctrl(var, expval, msg) \
  {                                  \
    if (var != expval)               \
    {                                \
      var = expval; \
      HAL_UART_Transmit(&huart3, char msg, sizeof(msg)), 100); \
    }            \
  }

#else
#define DEBUG(var, expval, msg)
#endif
#define DEBUG_PRINT_LN(x) HAL_UART_Transmitte(&huart, (uint8_t*)(x), sizeof(x), 100) //Serial.println
#define DEBUG_PRINT_1(x)  HAL_UART_Transmitte(&huart, (uint8_t*)(x), sizeof(x), 100) //Serial.println
#define DEBUG_PRINT(x)  // HAL_UART_Transmitte(&huart, (uint8_t*)(x), sizeof(x), 100) // Serial.printf
#define ESPCOM_PRINT(x)  HAL_UART_Transmitte(&huart, (uint8_t*)(x), sizeof(x), 100)  //Serial.println
#define CAB_PRINT(x)  HAL_UART_Transmitte(&huart, (uint8_t*)(x), sizeof(x), 100) //Serial.println
#define CAB_PRINT_1(x)  HAL_UART_Transmitte(&huart, (uint8_t*)(x), sizeof(x), 100) //Serial.print

// cab_print_1()
// serial.print()
// cab_print_1(x)
// 

// #ifdef __cplusplus
//  extern "C" {
// #endif /* __cplusplus */

void debugInit();

// #ifdef __cplusplus
// }
// #endif /* __cplusplus */

#endif /* DEBUG_H_ */
