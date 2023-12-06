/**
 * \file            main.c
 *
 * Cortex-M4 main.c file
 */

/* Includes ------------------------------------------------------------------*/

#define PERIPH_BASE    (0x40000000UL)
#define AHB4PERIPH_GPIOA_OFFSET 	(0x18020000UL)
#define AHB4PERIPH_GPIOB_OFFSET  	(0x18020400UL)
#define AHB4PERIPH_GPIOE_OFFSET 	(0x18021000UL)
#define GPIOA_BASE 	(PERIPH_BASE + AHB4PERIPH_GPIOA_OFFSET)
#define GPIOB_BASE 	(PERIPH_BASE + AHB4PERIPH_GPIOB_OFFSET)
#define GPIOE_BASE 	(PERIPH_BASE + AHB4PERIPH_GPIOE_OFFSET)
#define RCC_OFFSET 		(0x18024400UL)
#define RCC_BASE		(PERIPH_BASE + RCC_OFFSET)
#define AHB4ENR_OFFSET 	(0x0E0UL)
#define RCC_AHB4ENR		(*(volatile unsigned int *)(RCC_BASE + AHB4ENR_OFFSET))
#define MODER_OFFSET 	(0x00UL)
#define GPIOA_MODER		(*(volatile unsigned int *)(GPIOA_BASE + MODER_OFFSET))
#define GPIOB_MODER		(*(volatile unsigned int *)(GPIOB_BASE + MODER_OFFSET))
#define GPIOE_MODER		(*(volatile unsigned int *)(GPIOE_BASE + MODER_OFFSET))
#define	GPIO_ODR_OFFSET	(0x14UL)
#define GPIOA_ODR 		(*(volatile unsigned int *)(GPIOA_BASE + GPIO_ODR_OFFSET))
#define GPIOB_ODR 		(*(volatile unsigned int *)(GPIOB_BASE + GPIO_ODR_OFFSET))
#define GPIOE_ODR 		(*(volatile unsigned int *)(GPIOE_BASE + GPIO_ODR_OFFSET))
#define GPIOAEN (1U<<0)
#define GPIOBEN (1U<<1)
#define GPIOEEN (1U<<4)
#define PIN0			(1U<<0)
#define PIN1			(1U<<1)
#define PIN5			(1U<<5)
#define PIN14			(1U<<14)
#define LED_GREEN    	PIN0
#define LED_YELLOW	    PIN1
#define LED_PIN         PIN5
#define LED_RED			PIN14


#include "common.h"
#include "ringbuff.h"

void delay_time();

volatile ringbuff_t* rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
volatile ringbuff_t* rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;
static void led_init(void);


int main(void)
{
	 RCC_AHB4ENR |= GPIOBEN;

	 GPIOB_MODER |= (1U<<0);
	 GPIOB_MODER &= ~(1U<<1);

	 GPIOB_MODER |= (1U<<28);
	 GPIOB_MODER &= ~(1U<<29);

    while (!ringbuff_is_ready(rb_cm4_to_cm7) || !ringbuff_is_ready(rb_cm7_to_cm4)) {}

    /* Write message to buffer */
    ringbuff_write(rb_cm4_to_cm7, "[CM4] Core ready\r\n", 18);

    /* Set default time */

    while (1)
    {
        size_t len;
        void* addr;


        /* Send data to CPU1 */
        for(int i=0; i<10; i++)
        {

        }
           char c = 1;
            /* Write to buffer from CPU2 to CPU1 */
            ringbuff_write(rb_cm4_to_cm7, "[CM4] Number: ", 14);
            ringbuff_write(rb_cm4_to_cm7, &c, 1);
            ringbuff_write(rb_cm4_to_cm7, "\r\n", 2);

            GPIOB_ODR ^= LED_GREEN;	  //PB0
            	GPIOB_ODR ^= LED_RED;

            	delay_time();



        while ((len = ringbuff_get_linear_block_read_length(rb_cm7_to_cm4)) > 0)
        {
            addr = ringbuff_get_linear_block_read_address(rb_cm7_to_cm4);
            /* Mark buffer as read to allow other writes from CPU1 */
            ringbuff_skip(rb_cm7_to_cm4, len);
        }
    }
}

void delay_time()
{
	for(int i = 0; i<500000; i++)
	{

	}

}

