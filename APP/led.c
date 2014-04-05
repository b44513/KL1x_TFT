/* Basic led driver by neo.xiong@freescale.com */

#include "common.h"
#include "led.h"

/*  
    func name:  led_init 
    input:      none
    output:     none
    note:       init LED service
*/
void led_init(void)
{
     SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
     
     PORTB_PCR0  = PORT_PCR_MUX(1);
     FGPIOB_PDDR |= 1 << 0;
     FGPIOB_PSOR = 1 << 0;
}

/*  
    func name:  led_ctrl 
    input:      ON/OFF
    output:     none
    note:       ctrl the state of LED
*/
void led_ctrl(uint32 on_off)
{
    if (on_off == ON) {
        FGPIOB_PCOR = 1 << 0;
    } else {
        FGPIOB_PSOR = 1 << 0;
    }
}

/*  
    func name:  led_toggle 
    input:      none
    output:     none
    note:       toggle the state of LED
*/
void led_toggle(void)
{
    FGPIOB_PTOR = 1 << 0;
}