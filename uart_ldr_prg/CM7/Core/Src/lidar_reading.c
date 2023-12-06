/*
 * lidar_reading.c
 *
 *  Created on: Oct 30, 2023
 *      Author: Bhargav-4836
 */


#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>

uint8_t tfminiData[9];
int distance = 0;
int strength = 0;



void processLidarValue()
{
	static uint8_t i = 0;
	uint8_t j = 0;
	int checksum = 0;

	HAL_UART_Receive(&huart5, &tfminiData[i], 1, 10);
	{
	  //HAL_UART_Transmit(&huart3, tfminiData[i], 1, 100);
		if (tfminiData[0] != 0x59)
		{
			i = 0;
		}
		else if (i == 1 && tfminiData[1] != 0x59)
		{
			i = 0;
		}
		else if (i == 8)
		{
			for (j = 0; j < 8; j++)
			{
				checksum += tfminiData[j];
			}
			if (tfminiData[8] == (checksum % 256))
			{
				 distance = tfminiData[2] + (tfminiData[3] << 8);
				 strength = tfminiData[4] + (tfminiData[5] << 8);
			}

			i = 0;
		}
		else
		{
			i++;
		}
	}
}


void getTFminiData()
{

	char buffer[20];
    snprintf(buffer, sizeof(buffer),"Distance: %d cm\t", distance);
    HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 20);
    HAL_Delay(100);

}


void setUpLiudarDMA()
{
	if (HAL_UART_Receive_DMA(&huart5, tfminiData, sizeof(tfminiData)) != HAL_OK)
	  {
		  Error_Handler();
	  }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

        processLidarValue();
}

