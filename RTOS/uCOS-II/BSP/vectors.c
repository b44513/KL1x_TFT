/******************************************************************************
* File:    vectors.c
*
* Purpose: Configure interrupt vector table for Kinetis.
******************************************************************************/

#include <ucos_ii.h>
#include "bsp.h"

extern void __startup(void);
extern unsigned long __BOOT_STACK_ADDRESS[];

/******************************************************************************
* Vector Table
******************************************************************************/
typedef void (*vector_entry)(void);

void NMI_isr(void);
void hardfault_isr(void);
void spurious_isr(void);

#define VECTOR_PADDING  (vector_entry)0xffffffff
#define CONFIG_1		(vector_entry)0xffffffff 
#define CONFIG_2		(vector_entry)0xffffffff 
#define CONFIG_3		(vector_entry)0xffffffff
#define CONFIG_4		(vector_entry)0xfffffffe //b5=1,b4=1,b0=1 div1 fast
//#define CONFIG_4	(vector_entry)0xffffdffe //b5=0,b4=1,b0=1 div1 slow works
//#define CONFIG_4	(vector_entry)0xffffcefe //b5=0,b4=0,b0=0;div8 slow
//#define CONFIG_4	(vector_entry)0xffffeefe //b5=1,b4=0,b0=0 div8 fast 
//#define CONFIG_4	(vector_entry)0xffffcffe //b5=0,b4=0,b0=1;div4 slow
//#define CONFIG_4	(vector_entry)0xffffeffe //b5=1,b4=0,b0=1;div4 fast
//#define CONFIG_4	(vector_entry)0xfffffefe //b5=1,b4=1,b0=0;div2 fast
//#define CONFIG_4	(vector_entry)0xffffdefe //b5=0,b4=1,b0=0;div2 slow 

const vector_entry __vector_table[] @ ".intvec" =
{
    (vector_entry)__BOOT_STACK_ADDRESS,       /* Initial SP           */
    __startup,                                /* Initial PC           */
    NMI_isr,                                  
    hardfault_isr, 
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    spurious_isr,
    OS_CPU_PendSVHandler,
    OS_CPU_SysTickHandler,
    
    DMA_ch0_isr,
    DMA_ch1_isr,
    DMA_ch2_isr,
    DMA_ch3_isr,
    spurious_isr,
    FTFA_isr,
    PMC_isr,
    LLWU_isr,
    I2C0_isr,
    I2C1_isr,
    SPI0_isr,
    SPI1_isr,
    UART0_isr,
    UART1_isr,
    UART2_isr,
    ADC0_isr,
    CMP0_isr,
    TPM0_isr,
    TPM1_isr,
    TPM2_isr,
    RTC_alarm_isr,
    RTC_seconds_isr,
    PIT_isr,
    I2S0_isr,
    spurious_isr,
    DAC0_isr,
    TSI0_isr,
    MCG_isr,
    LPTMR0_isr,
    spurious_isr,
    PORTA_isr,
    PORTCD_isr,
    
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    VECTOR_PADDING,
    
    CONFIG_1,
    CONFIG_2,
    CONFIG_3,
    CONFIG_4,
};

/******************************************************************************
* default_isr(void)
*
* Default ISR definition.
*
* In:  n/a
* Out: n/a
******************************************************************************/
void NMI_isr(void)
{
    while (1) {
        ;
    }
}

/******************************************************************************
* hardfault_isr(void)
*
* Hardfault ISR definition.
*
* In:  n/a
* Out: n/a
******************************************************************************/
void hardfault_isr(void)
{
    while (1) {
        ;
    }
}

/******************************************************************************
* spurious_isr(void)
*
* Spurious ISR definition.
*
* In:  n/a
* Out: n/a
******************************************************************************/
void spurious_isr(void)
{
    while (1) {
        ;
    }
}


