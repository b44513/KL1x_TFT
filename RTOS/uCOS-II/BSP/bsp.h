/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*                 MD
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  __BSP_H__
#define  __BSP_H__

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#include <ucos_ii.h>
#include <common.h>

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <stdarg.h>

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

typedef  void (*CPU_FNCT_VOID)(void);

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/

#define  BSP_DMA_ch0_irq_no         0   // Vector No 16
#define  BSP_DMA_ch1_irq_no         1   // Vector No 17
#define  BSP_DMA_ch2_irq_no         2   // Vector No 18
#define  BSP_DMA_ch3_irq_no         3   // Vector No 19
#define  BSP_Reserved1_irq_no       4   // Vector No 20
#define  BSP_FTFA_irq_no            5   // Vector No 21
#define  BSP_PMC_irq_no             6   // Vector No 22
#define  BSP_LLWU_irq_no            7   // Vector No 23
#define  BSP_I2C0_irq_no            8   // Vector No 24
#define  BSP_I2C1_irq_no            9   // Vector No 25
#define  BSP_SPI0_irq_no            10  // Vector No 26
#define  BSP_SPI1_irq_no            11  // Vector No 27
#define  BSP_UART0_irq_no           12  // Vector No 28
#define  BSP_UART1_irq_no           13  // Vector No 29
#define  BSP_UART2_irq_no           14  // Vector No 30
#define  BSP_ADC0_irq_no            15  // Vector No 31
#define  BSP_CMP0_irq_no            16  // Vector No 32
#define  BSP_FTM0_irq_no            17  // Vector No 33
#define  BSP_FTM1_irq_no            18  // Vector No 34
#define  BSP_FTM2_irq_no            19  // Vector No 35
#define  BSP_RTC_alarm_irq_no       20  // Vector No 36
#define  BSP_RTC_seconds_irq_no     21  // Vector No 37
#define  BSP_PIT_irq_no             22  // Vector No 38
#define  BSP_I2S0_irq_no            23  // Vector No 39
#define  BSP_Reserved2_irq_no       24  // Vector No 20
#define  BSP_DAC0_irq_no            25  // Vector No 41
#define  BSP_TSI0_irq_no            26  // Vector No 42
#define  BSP_MCG_irq_no             27  // Vector No 43
#define  BSP_LPTMR0_irq_no          28  // Vector No 44
#define  BSP_Reserved3_irq_no       29  // Vector No 20
#define  BSP_PORTA_irq_no           30  // Vector No 46
#define  BSP_PORTCD_irq_no          31  // Vector No 47

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init                          (void);
void        BSP_IntDisAll                     (void);
INT32U      BSP_CPU_ClkFreq                   (void);

/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/

void        BSP_IntInit                       (void);

void        BSP_IntEn                         (INT32U       int_id);

void        BSP_IntDis                        (INT32U       int_id);

void        BSP_IntVectSet                    (INT32U       int_id,
                                               CPU_FNCT_VOID  isr);

void        BSP_IntPrioSet                    (INT32U       int_id,
                                               INT8U        prio);

void DMA_ch0_isr (void);
void DMA_ch1_isr (void);
void DMA_ch2_isr (void);
void DMA_ch3_isr (void);
void spurious_isr (void);
void FTFA_isr (void);
void PMC_isr (void);
void LLWU_isr (void);
void I2C0_isr (void);
void I2C1_isr (void);
void SPI0_isr (void);
void SPI1_isr (void);
void UART0_isr (void);
void UART1_isr (void);
void UART2_isr (void);
void ADC0_isr (void);
void CMP0_isr (void);
void TPM0_isr (void);
void TPM1_isr (void);
void TPM2_isr (void);
void RTC_alarm_isr (void);
void RTC_seconds_isr (void);
void PIT_isr (void);
void I2S0_isr (void);
void spurious_isr (void);
void DAC0_isr (void);
void TSI0_isr (void);
void MCG_isr (void);
void LPTMR0_isr (void);
void spurious_isr (void);
void PORTA_isr (void);
void PORTCD_isr (void);

/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */

