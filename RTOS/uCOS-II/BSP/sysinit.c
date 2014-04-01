/*
 * File:        sysinit.c
 * Purpose:     Kinetis L Family Configuration
 *              Initializes processor to a default state
 *
 * Notes:
 *
 *
 */

#include "common.h"
#include "uart.h"
#include "mcg.h"
#include "sysinit.h"

/********************************************************************/

/* System clock frequency variables - Represents the current system clock
 * settings
 */
int mcg_clk_hz;
int mcg_clk_khz;
int core_clk_khz;
int periph_clk_khz;
int uart0_clk_khz;

/********************************************************************/
void sysinit (void)
{
    /* Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK);
        
    // Release hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
    // if ACKISO is set you must clear ackiso before calling pll_init 
    //    or pll init hangs waiting for OSC to initialize
    // if osc enabled in low power modes - enable it first before ack
    // if I/O needs to be maintained without glitches enable outputs and modules first before ack.
    if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK) {
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;
    }

#if defined(NO_PLL_INIT)
        mcg_clk_hz = 21000000; //FEI mode
        
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK; // clear PLLFLLSEL to select the FLL for this clock source
        
        uart0_clk_khz = (mcg_clk_hz / 1000); // the uart0 clock frequency will equal the FLL frequency
#else 
       /* Ramp up the system clock */
       /* Set the system dividers */
       /* NOTE: The PLL init will not configure the system clock dividers,
        * so they must be configured appropriately before calling the PLL
        * init function to ensure that clocks remain in valid ranges.
        */  
        SIM_CLKDIV1 = ( 0
                        | SIM_CLKDIV1_OUTDIV1(0)
                        | SIM_CLKDIV1_OUTDIV4(1) );
 
       /* Initialize PLL */
       /* PLL will be the source for MCG CLKOUT so the core, system, and flash clocks are derived from it */ 
       mcg_clk_hz = pll_init(8000000,       /* CLKIN0 frequency */
                             LOW_POWER,     /* Set the oscillator for low power mode */
                             CRYSTAL,       /* Crystal or canned oscillator clock input */
                             4,             /* PLL predivider value */
                             24,            /* PLL multiplier */
                             MCGOUT);       /* Use the output from this PLL as the MCGOUT */

       /* Check the value returned from pll_init() to make sure there wasn't an error */
       if (mcg_clk_hz < 0x100)
         while(1);
       
       SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source
       
       uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
#endif      

    /*
    * Use the value obtained from the pll_init function to define variables
    * for the core clock in kHz and also the peripheral clock. These
    * variables can be used by other functions that need awareness of the
    * system frequency.
    */
    mcg_clk_khz = mcg_clk_hz / 1000;
    core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
    periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> 16)+ 1);

    /* init debug UART */
    PORTA_PCR1 = PORT_PCR_MUX(0x2);
    PORTA_PCR2 = PORT_PCR_MUX(0x2);
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);
    uart0_init (UART0_BASE_PTR, uart0_clk_khz, 115200);
}

unsigned int sys_get_core_freq_in_hz(void)
{
    return (core_clk_khz * 1000);
}
