/*
 * shared_mem_handler.c
 *
 *  Created on: Oct 18, 2023
 *      Author: Bhargav-4836
 */

#include "ringbuff.h"
#include "common.h"
#include "shared_mem_handler.h"
#include "usart.h"



volatile ringbuff_t* rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
volatile ringbuff_t* rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;


void printshared();

void printshared()
{
	uint32_t time, t1;

	HAL_UART_Transmit(&huart3, (uint8_t *)"[CM7] Core ready\r\n", sizeof("[CM7] Core ready\r\n"), 1000);


	time = t1 = HAL_GetTick();

	while (1)
	{
	    size_t len;
	    void* addr;

	    time = HAL_GetTick();
	   // char buf[10];

	    len = ringbuff_get_linear_block_read_length(rb_cm4_to_cm7);
	    while (len > 0)
	    {
	        addr = ringbuff_get_linear_block_read_address(rb_cm4_to_cm7);

	     //   int read = ringbuff_read(rb_cm4_to_cm7, addr, 1);

//	        int size = snprintf(buf, sizeof(buf), "size %d", read);

	        HAL_UART_Transmit(&huart3, addr, len, 1000);
	        ringbuff_skip(rb_cm4_to_cm7, len);


	    }

        /* Toggle LED */
		if (time - t1 >= 500)
		{
			t1 = time;
			HAL_GPIO_TogglePin(led2_GPIO_Port, led2_Pin);
		}



//	    ringbuff_write(rb_cm7_to_cm4, "my_data", 7);
	}
}


//void send_data_ringbuffer(shared_mem_data sh_dt)
//{
//	while (!ringbuff_is_ready(rb_cm4_to_cm7) || !ringbuff_is_ready(rb_cm7_to_cm4)){}
//
//	ringbuff_write(rb_cm4_to_cm7, (uint8_t *)sh_dt, sizeof(sh_dt));
//}


