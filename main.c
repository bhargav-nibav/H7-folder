#define PERIPH_BASE    (0x40000000UL)

//0x58020000 - 0x580203FF GPIOA // as well as AHB4 starting address
//AHB4 starting address 0x40000000 + 18020000 = 0x58020000
//0x58020400 - 0x580207FF GPIOB
//0x58021000 - 0x580213FF GPIOE

#define AHB4PERIPH_GPIOA_OFFSET 	(0x18020000UL)
#define AHB4PERIPH_GPIOB_OFFSET  	(0x18020400UL)
#define AHB4PERIPH_GPIOE_OFFSET 	(0x18021000UL)


#define GPIOA_BASE 	(PERIPH_BASE + AHB4PERIPH_GPIOA_OFFSET)
#define GPIOB_BASE 	(PERIPH_BASE + AHB4PERIPH_GPIOB_OFFSET)
#define GPIOE_BASE 	(PERIPH_BASE + AHB4PERIPH_GPIOE_OFFSET)


//0x58024400 - 0x580247FF RCC

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

void delay_time();

int main (void)
{

	/*1. Enable the clock acess to GPIOs
	  2. Set pins as output pins
	  */
	  RCC_AHB4ENR |= GPIOAEN;
	  RCC_AHB4ENR |= GPIOBEN;
	  RCC_AHB4ENR |= GPIOEEN;


//	  GPIOA_MODER |= (1U<<10);
//	  GPIOA_MODER &= ~(1U<<11);  //GPIOA pin 5 setting into output mode

	  GPIOB_MODER |= (1U<<0);
	  GPIOB_MODER &= ~(1U<<1);    //GPIOB pin 0 setting into output mode

	  GPIOB_MODER |= (1U<<28);
	  GPIOB_MODER &= ~(1U<<29);   //GPIOB pin 14 setting into output mode

	  GPIOE_MODER |= (1U<<2);
	  GPIOE_MODER &= ~(1U<<3);    //GPIOE pin 1 setting into output mode



	  while(1)
	  {
		  /*3. Set the pins to high*/
//		  GPIOA_ODR |= LED_PIN;       //PA5
//		  GPIOB_ODR |= LED_GREEN;	  //PB0
//		  GPIOB_ODR |= LED_RED;		  //PB14
//		  GPIOE_ODR |= LED_YELLOW;	  //PE1

		  /*Toggle pin*/

		  GPIOA_ODR ^= LED_PIN;       //PA5
		  GPIOB_ODR ^= LED_GREEN;	  //PB0
		  GPIOB_ODR ^= LED_RED;		  //PB14
		  GPIOE_ODR ^= LED_YELLOW;	  //PE1
		  delay_time();


	  }
}

void delay_time()
{
	for(int i = 0; i<10000000; i++)
	{

	}
}