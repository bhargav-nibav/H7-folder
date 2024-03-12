
#include "ringbuff.h"
#include "common.h"


typedef struct
{
  __IO uint32_t R[32];      /*!< 2-step write lock and read back registers,     Address offset: 00h-7Ch  */
  __IO uint32_t RLR[32];    /*!< 1-step read lock registers,                    Address offset: 80h-FCh  */
  __IO uint32_t C1IER;      /*!< HSEM Interrupt 0 enable register ,             Address offset: 100h     */
  __IO uint32_t C1ICR;      /*!< HSEM Interrupt 0 clear register ,              Address offset: 104h     */
  __IO uint32_t C1ISR;      /*!< HSEM Interrupt 0 Status register ,             Address offset: 108h     */
  __IO uint32_t C1MISR;     /*!< HSEM Interrupt 0 Masked Status register ,      Address offset: 10Ch     */
  __IO uint32_t C2IER;      /*!< HSEM Interrupt 1 enable register ,             Address offset: 110h     */
  __IO uint32_t C2ICR;      /*!< HSEM Interrupt 1 clear register ,              Address offset: 114h     */
  __IO uint32_t C2ISR;      /*!< HSEM Interrupt 1 Status register ,             Address offset: 118h     */
  __IO uint32_t C2MISR;     /*!< HSEM Interrupt 1 Masked Status register ,      Address offset: 11Ch     */
   uint32_t  Reserved[8];   /* Reserved                                         Address offset: 120h-13Ch*/
  __IO uint32_t CR;         /*!< HSEM Semaphore clear register ,                Address offset: 140h      */
  __IO uint32_t KEYR;       /*!< HSEM Semaphore clear key register ,            Address offset: 144h      */

} HSEM_TypeDef;


typedef struct
{
  __IO uint32_t IER;        /*!< HSEM interrupt enable register ,                Address offset:   0h     */
  __IO uint32_t ICR;        /*!< HSEM interrupt clear register ,                 Address offset:   4h     */
  __IO uint32_t ISR;        /*!< HSEM interrupt status register ,                Address offset:   8h     */
  __IO uint32_t MISR;       /*!< HSEM masked interrupt status register ,         Address offset:   Ch     */
} HSEM_Common_TypeDef;



typedef struct
{
 __IO uint32_t CR;             //!< RCC clock control register,                                              Address offset: 0x00  
 __IO uint32_t HSICFGR;        //!< HSI Clock Calibration Register,                                          Address offset: 0x04  
 __IO uint32_t CRRCR;          //!< Clock Recovery RC  Register,                                             Address offset: 0x08  
 __IO uint32_t CSICFGR;        //!< CSI Clock Calibration Register,                                          Address offset: 0x0C  
 __IO uint32_t CFGR;           //!< RCC clock configuration register,                                        Address offset: 0x10  
 uint32_t     RESERVED1;       //!< Reserved,                                                                Address offset: 0x14  
 __IO uint32_t D1CFGR;         //!< RCC Domain 1 configuration register,                                     Address offset: 0x18  
 __IO uint32_t D2CFGR;         //!< RCC Domain 2 configuration register,                                     Address offset: 0x1C  
 __IO uint32_t D3CFGR;         //!< RCC Domain 3 configuration register,                                     Address offset: 0x20  
 uint32_t     RESERVED2;       //!< Reserved,                                                                Address offset: 0x24  
 __IO uint32_t PLLCKSELR;      //!< RCC PLLs Clock Source Selection Register,                                Address offset: 0x28  
 __IO uint32_t PLLCFGR;        //!< RCC PLLs  Configuration Register,                                        Address offset: 0x2C  
 __IO uint32_t PLL1DIVR;       //!< RCC PLL1 Dividers Configuration Register,                                Address offset: 0x30  
 __IO uint32_t PLL1FRACR;      //!< RCC PLL1 Fractional Divider Configuration Register,                      Address offset: 0x34  
 __IO uint32_t PLL2DIVR;       //!< RCC PLL2 Dividers Configuration Register,                                Address offset: 0x38  
 __IO uint32_t PLL2FRACR;      //!< RCC PLL2 Fractional Divider Configuration Register,                      Address offset: 0x3C  
 __IO uint32_t PLL3DIVR;       //!< RCC PLL3 Dividers Configuration Register,                                Address offset: 0x40  
 __IO uint32_t PLL3FRACR;      //!< RCC PLL3 Fractional Divider Configuration Register,                      Address offset: 0x44  
 uint32_t      RESERVED3;      //!< Reserved,                                                                Address offset: 0x48  
 __IO uint32_t  D1CCIPR;       //!< RCC Domain 1 Kernel Clock Configuration Register                         Address offset: 0x4C  
 __IO uint32_t  D2CCIP1R;      //!< RCC Domain 2 Kernel Clock Configuration Register                         Address offset: 0x50  
 __IO uint32_t  D2CCIP2R;      //!< RCC Domain 2 Kernel Clock Configuration Register                         Address offset: 0x54  
 __IO uint32_t  D3CCIPR;       //!< RCC Domain 3 Kernel Clock Configuration Register                         Address offset: 0x58  
 uint32_t      RESERVED4;      //!< Reserved,                                                                Address offset: 0x5C  
 __IO uint32_t  CIER;          //!< RCC Clock Source Interrupt Enable Register                               Address offset: 0x60  
 __IO uint32_t  CIFR;          //!< RCC Clock Source Interrupt Flag Register                                 Address offset: 0x64  
 __IO uint32_t  CICR;          //!< RCC Clock Source Interrupt Clear Register                                Address offset: 0x68  
 uint32_t     RESERVED5;       //!< Reserved,                                                                Address offset: 0x6C  
 __IO uint32_t  BDCR;          //!< RCC Vswitch Backup Domain Control Register,                              Address offset: 0x70  
 __IO uint32_t  CSR;           //!< RCC clock control & status register,                                     Address offset: 0x74  
 uint32_t     RESERVED6;       //!< Reserved,                                                                Address offset: 0x78  
 __IO uint32_t AHB3RSTR;       //!< RCC AHB3 peripheral reset register,                                      Address offset: 0x7C  
 __IO uint32_t AHB1RSTR;       //!< RCC AHB1 peripheral reset register,                                      Address offset: 0x80  
 __IO uint32_t AHB2RSTR;       //!< RCC AHB2 peripheral reset register,                                      Address offset: 0x84  
 __IO uint32_t AHB4RSTR;       //!< RCC AHB4 peripheral reset register,                                      Address offset: 0x88  
 __IO uint32_t APB3RSTR;       //!< RCC APB3 peripheral reset register,                                      Address offset: 0x8C  
 __IO uint32_t APB1LRSTR;      //!< RCC APB1 peripheral reset Low Word register,                             Address offset: 0x90  
 __IO uint32_t APB1HRSTR;      //< RCC APB1 peripheral reset High Word register,                            Address offset: 0x94  
 __IO uint32_t APB2RSTR;       //!< RCC APB2 peripheral reset register,                                      Address offset: 0x98  
 __IO uint32_t APB4RSTR;       //!< RCC APB4 peripheral reset register,                                      Address offset: 0x9C  
 __IO uint32_t GCR;            //!< RCC RCC Global Control  Register,                                        Address offset: 0xA0  
 uint32_t     RESERVED8;       //!< Reserved,                                                                Address offset: 0xA4  
 __IO uint32_t D3AMR;          //!< RCC Domain 3 Autonomous Mode Register,                                   Address offset: 0xA8  
 uint32_t     RESERVED11[9];    //!< Reserved, 0xAC-0xCC                                                      Address offset: 0xAC 
 __IO uint32_t RSR;            //!< RCC Reset status register,                                               Address offset: 0xD0  
 __IO uint32_t AHB3ENR;        //!< RCC AHB3 peripheral clock  register,                                     Address offset: 0xD4  
 __IO uint32_t AHB1ENR;        //!< RCC AHB1 peripheral clock  register,                                     Address offset: 0xD8  
 __IO uint32_t AHB2ENR;        //!< RCC AHB2 peripheral clock  register,                                     Address offset: 0xDC  
 __IO uint32_t AHB4ENR;        //!< RCC AHB4 peripheral clock  register,                                     Address offset: 0xE0  
 __IO uint32_t APB3ENR;        //!< RCC APB3 peripheral clock  register,                                     Address offset: 0xE4  
 __IO uint32_t APB1LENR;       //!< RCC APB1 peripheral clock  Low Word register,                            Address offset: 0xE8  
 __IO uint32_t APB1HENR;       //!< RCC APB1 peripheral clock  High Word register,                           Address offset: 0xEC  
 __IO uint32_t APB2ENR;        //!< RCC APB2 peripheral clock  register,                                     Address offset: 0xF0  
 __IO uint32_t APB4ENR;        //!< RCC APB4 peripheral clock  register,                                     Address offset: 0xF4  
 uint32_t      RESERVED12;      //!< Reserved,                                                                Address offset: 0xF8 
 __IO uint32_t AHB3LPENR;      //!< RCC AHB3 peripheral sleep clock  register,                               Address offset: 0xFC  
 __IO uint32_t AHB1LPENR;      //!< RCC AHB1 peripheral sleep clock  register,                               Address offset: 0x100 
 __IO uint32_t AHB2LPENR;      //!< RCC AHB2 peripheral sleep clock  register,                               Address offset: 0x104 
 __IO uint32_t AHB4LPENR;      //!< RCC AHB4 peripheral sleep clock  register,                               Address offset: 0x108 
 __IO uint32_t APB3LPENR;      //!< RCC APB3 peripheral sleep clock  register,                               Address offset: 0x10C 
 __IO uint32_t APB1LLPENR;     //!< RCC APB1 peripheral sleep clock  Low Word register,                      Address offset: 0x110 
 __IO uint32_t APB1HLPENR;     //!< RCC APB1 peripheral sleep clock  High Word register,                     Address offset: 0x114 
 __IO uint32_t APB2LPENR;      //!< RCC APB2 peripheral sleep clock  register,                               Address offset: 0x118 
 __IO uint32_t APB4LPENR;      //!< RCC APB4 peripheral sleep clock  register,                               Address offset: 0x11C 
 uint32_t     RESERVED13[4];   //!< Reserved, 0x120-0x12C                                                    Address offset: 0x120 
} RCC_TypeDef;

#define DUAL_CORE 
 
#define PERIPH_BASE               (0x40000000UL)    

#define D3_APB1PERIPH_BASE       (PERIPH_BASE + 0x18000000UL)
#define D3_AHB1PERIPH_BASE       (PERIPH_BASE + 0x18020000UL)




#define RCC_BASE              (D3_AHB1PERIPH_BASE + 0x4400UL)
#define RCC_C1_BASE           (RCC_BASE + 0x130UL)
#define RCC_C2_BASE           (RCC_BASE + 0x190UL)

#define HSEM_BASE             (D3_AHB1PERIPH_BASE + 0x6400UL)

#define RCC                 ((RCC_TypeDef *) RCC_BASE)

#define RCC_C1              ((RCC_Core_TypeDef *) RCC_C1_BASE)
#define RCC_C2              ((RCC_Core_TypeDef *) RCC_C2_BASE)

#define RCC_AHB4ENR_HSEMEN_Pos                 (25U)
#define RCC_AHB4ENR_HSEMEN_Msk                 (0x1UL << RCC_AHB4ENR_HSEMEN_Pos)          //!< 0x02000000 
#define RCC_AHB4ENR_HSEMEN                     RCC_AHB4ENR_HSEMEN_Msk

 #define assert_param(expr) ((void)0U)


#define HSEM                ((HSEM_TypeDef *) HSEM_BASE)
#if defined(CORE_CM4)
#define HSEM_COMMON         ((HSEM_Common_TypeDef *) (HSEM_BASE + 0x110UL))
#else  /* CORE_CM7 */
#define HSEM_COMMON         ((HSEM_Common_TypeDef *) (HSEM_BASE + 0x100UL))
#endif /* CORE_CM4 */








#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))

#define UNUSED(x) ((void)(x))







/*
#include "ringbuff.h"
#include "common.h"
*/
volatile ringbuff_t* rb_cm4_to_cm7 = (void *)BUFF_CM4_TO_CM7_ADDR;
volatile ringbuff_t* rb_cm7_to_cm4 = (void *)BUFF_CM7_TO_CM4_ADDR;

int main()
{
	
	semaforceEnable();
	semaforceActiveNotification(HSEM_WAKEUP_CPU2_MASK);
	PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
    __HAL_HSEM_CLEAR_FLAG(HSEM_WAKEUP_CPU2_MASK);




}


	
#if defined(RCC_AHB4ENR_HSEMEN)
#define semaforceEnable() 			do { \
										__IO uint32_t tmpreg; \
										SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
										/* Delay after an RCC peripheral clock enabling */ \
										tmpreg = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_HSEMEN);\
										UNUSED(tmpreg); \
									} while(0)
}

#endif






void semaforceActiveNotification(uint32_t SemMask)
{
#if  USE_MULTI_CORE_SHARED_CODE != 0U
  /*enable the semaphore mask interrupts */
  if (GetCurrentCPUID() == HSEM_CPU1_COREID)
  {
    /*Use interrupt line 0 for CPU1 Master */
    HSEM->C1IER |= SemMask;
  }
  else /* HSEM_CPU2_COREID */
  {
    /*Use interrupt line 1 for CPU2 Master*/
    HSEM->C2IER |= SemMask;
  }
#else
	HSEM_COMMON->IER |= SemMask; ///////////////////// used in M4
#endif
}
	

#if defined(DUAL_CORE)

/**
  * @brief  Returns the current CPU ID.
  * @retval CPU identifier
  */
uint32_t GetCurrentCPUID(void)
{
  if (((SCB->CPUID & 0x000000F0U) >> 4 )== 0x7U)
  {
    return  CM7_CPUID;
  }
  else
  {
    return CM4_CPUID;
  }
}

#else

/**
* @brief  Returns the current CPU ID.
* @retval CPU identifier
*/
uint32_t HAL_GetCurrentCPUID(void)
{
  return  CM7_CPUID;
}

#endif /*DUAL_CORE*/




void PWREx_ClearPendingEvent (void)
{
#if defined (DUAL_CORE)
  /* Check the current Core */
  if (HAL_GetCurrentCPUID () == CM7_CPUID)
  {
    __WFE ();
  }
  else
  {
    __SEV ();
    __WFE ();
  }
#else
  __WFE ();  ///////////// used for M4
#endif /* defined (DUAL_CORE) */
}
