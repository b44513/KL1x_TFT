/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
* Filename      : bsp_int.c
* Version       : V1.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  ARM_INTERRUPT_LEVEL_BITS                       2
#define  BSP_INT_SRC_NBR                               32     /* Available Interrupts in Design                       */

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandler     (INT32U  int_id);
static  void  BSP_IntHandlerDummy(void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (INT32U int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        NVIC_ICER = 1 << (int_id % 32);
    }
}

/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    asm("CPSID   I");
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (INT32U int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        NVIC_ICPR |= 1 << (int_id % 32);
        NVIC_ISER |= 1 << (int_id % 32);
    }
}

/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (INT32U         int_id,
                      CPU_FNCT_VOID  isr)
{
#if OS_CRITICAL_METHOD == 3u                                   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif

    if (int_id < BSP_INT_SRC_NBR) {
        
        OS_ENTER_CRITICAL();
        BSP_IntVectTbl[int_id] = isr;
        OS_EXIT_CRITICAL();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (INT32U    int_id,
                      INT8U    prio)
{
#if OS_CRITICAL_METHOD == 3u                                   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    INT8U *prio_reg;
    INT8U div = 0;

    if ((int_id < BSP_INT_SRC_NBR) && (prio <= 3)) {
        OS_ENTER_CRITICAL();
        div = int_id / 4;
        prio_reg = (INT8U *)((INT32U)&NVIC_IP(div));
        /* Assign priority to IRQ */
        *prio_reg = ((prio & 0x3) << (8 - ARM_INTERRUPT_LEVEL_BITS));  
        OS_EXIT_CRITICAL();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    INT32U int_id;

    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void    DMA_ch0_isr                                 (void)    { BSP_IntHandler(BSP_DMA_ch0_irq_no);                         }
void    DMA_ch1_isr                                 (void)    { BSP_IntHandler(BSP_DMA_ch1_irq_no);                         }
void    DMA_ch2_isr                                 (void)    { BSP_IntHandler(BSP_DMA_ch2_irq_no);                         }
void    DMA_ch3_isr                                 (void)    { BSP_IntHandler(BSP_DMA_ch3_irq_no);                         }
void    FTFA_isr                                    (void)    { BSP_IntHandler(BSP_FTFA_irq_no);                            }
void    PMC_isr                                     (void)    { BSP_IntHandler(BSP_PMC_irq_no);                             }
void    LLWU_isr                                    (void)    { BSP_IntHandler(BSP_LLWU_irq_no);                            }
void    I2C0_isr                                    (void)    { BSP_IntHandler(BSP_I2C0_irq_no);                            }
void    I2C1_isr                                    (void)    { BSP_IntHandler(BSP_I2C1_irq_no);                            }
void    SPI0_isr                                    (void)    { BSP_IntHandler(BSP_SPI0_irq_no);                            }
void    SPI1_isr                                    (void)    { BSP_IntHandler(BSP_SPI1_irq_no);                            }
void    UART0_isr                                   (void)    { BSP_IntHandler(BSP_UART0_irq_no);                           }
void    UART1_isr                                   (void)    { BSP_IntHandler(BSP_UART1_irq_no);                           }
void    UART2_isr                                   (void)    { BSP_IntHandler(BSP_UART2_irq_no);                           }
void    ADC0_isr                                    (void)    { BSP_IntHandler(BSP_ADC0_irq_no);                            }
void    CMP0_isr                                    (void)    { BSP_IntHandler(BSP_CMP0_irq_no);                            }
void    TPM0_isr                                    (void)    { BSP_IntHandler(BSP_FTM0_irq_no);                            }
void    TPM1_isr                                    (void)    { BSP_IntHandler(BSP_FTM1_irq_no);                            }
void    TPM2_isr                                    (void)    { BSP_IntHandler(BSP_FTM2_irq_no);                            }
void    RTC_alarm_isr                               (void)    { BSP_IntHandler(BSP_RTC_alarm_irq_no);                       }
void    RTC_seconds_isr                             (void)    { BSP_IntHandler(BSP_RTC_seconds_irq_no);                     }
void    PIT_isr                                     (void)    { BSP_IntHandler(BSP_PIT_irq_no);                             }
void    I2S0_isr                                    (void)    { BSP_IntHandler(BSP_I2S0_irq_no);                            }
void    DAC0_isr                                    (void)    { BSP_IntHandler(BSP_DAC0_irq_no);                            }
void    TSI0_isr                                    (void)    { BSP_IntHandler(BSP_TSI0_irq_no);                            }
void    MCG_isr                                     (void)    { BSP_IntHandler(BSP_MCG_irq_no);                             }
void    LPTMR0_isr                                  (void)    { BSP_IntHandler(BSP_LPTMR0_irq_no);                          }
void    PORTA_isr                                   (void)    { BSP_IntHandler(BSP_PORTA_irq_no);                           }
void    PORTCD_isr                                  (void)    { BSP_IntHandler(BSP_PORTCD_irq_no);                          }

/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (INT32U  int_id)
{
    CPU_FNCT_VOID  isr;
    
#if OS_CRITICAL_METHOD == 3u                                   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif

    OS_ENTER_CRITICAL();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    OS_EXIT_CRITICAL();

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];

        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
}

/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{
    while (OS_TRUE) {
        ;
    }
}
