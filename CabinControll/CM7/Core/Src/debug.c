/*
 * debug.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Bhargav-4836
 */


// #include "debug.h"

#include <usart.h>

void debugInit()
{
	MX_USART3_UART_Init();
	  MX_UART4_Init();
}
