#include "ringbuff.h"
#include "common.h"
#include "shared_mem_handler.h"


volatile ringbuff_t* rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
volatile ringbuff_t* rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;

void send_data_ringbuffer(shared_mem_data sh_dt)
{
	uint32_t i = 0, time, t1, t2;
//	while (!ringbuff_is_ready(rb_cm4_to_cm7) || !ringbuff_is_ready(rb_cm7_to_cm4)){}
//
//	ringbuff_write(rb_cm4_to_cm7, "[CM4] Number: ", 14);
//	ringbuff_write(rb_cm4_to_cm7, (uint8_t *)sh_dt, 1);
//}

ringbuff_write(rb_cm4_to_cm7, "[CM4] Core ready\r\n", 18);

    /* Set default time */
    time = t1 = t2 = HAL_GetTick();
    while (1) {
        size_t len;
        void* addr;

        time = HAL_GetTick();

        /* Send data to CPU1 */
        if (time - t1 >= 1000)
        {
            t1 = time;
            char c = '0' + (++i % 10);

            /* Write to buffer from CPU2 to CPU1 */
            ringbuff_write(rb_cm4_to_cm7, "[CM4] Number: ", 14);
            ringbuff_write(rb_cm4_to_cm7, &c, 1);
            ringbuff_write(rb_cm4_to_cm7, "\r\n", 2);
        }

        /* Toggle LED */
        if (time - t2 >= 500)
        {
            t2 = time;
//            HAL_GPIO_TogglePin(LD3_GPIO_PORT, LD3_GPIO_PIN);
        }

        /* Check if CPU1 sent some data to CPU2 core */
        while ((len = ringbuff_get_linear_block_read_length(rb_cm7_to_cm4)) > 0) {
            addr = ringbuff_get_linear_block_read_address(rb_cm7_to_cm4);

            /*
             * `addr` holds pointer to beginning of data array
             * which can be used directly in linear form.
             *
             * Its length is `len` bytes
             */
            /* Process data here */
          //  HAL_UART_Transmit(&huart3, addr, len, 1000);

            /* Mark buffer as read to allow other writes from CPU1 */
            ringbuff_skip(rb_cm7_to_cm4, len);
        }
    }
}
