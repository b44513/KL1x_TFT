/* ILI9341 GPIO driver by neo.xiong@freescale.com */

#include "common.h"
#include "lcd.h"

/* 
    macro define area
*/

#define _RST_H()    (FGPIOB_PSOR = 1 << 2)
#define _RST_L()	(FGPIOB_PCOR = 1 << 2)

#define _BL_H()     (FGPIOB_PSOR = 1 << 3)
#define _BL_L()	    (FGPIOB_PCOR = 1 << 3)

#define _CS_H()	    (FGPIOB_PSOR = 1 << 16)
#define _CS_L()	    (FGPIOB_PCOR = 1 << 16)

#define _RS_H()	    (FGPIOB_PSOR = 1 << 17)
#define _RS_L()	    (FGPIOB_PCOR = 1 << 17)

#define _WR_H()	    (FGPIOB_PSOR = 1 << 18)
#define _WR_L()	    (FGPIOB_PCOR = 1 << 18)

#define _RD_H()	    (FGPIOB_PSOR = 1 << 19)
#define _RD_L()	    (FGPIOB_PCOR = 1 << 19)


#define _DATA(val)                  do {                   \
                                        FGPIOC_PDOR = val; \
                                        FGPIOD_PDOR = (val) >> 12;  \
                                    } while (0)            \

                                        
#define LCD_WRITE_CMD(val)          do {                \
                                        _CS_L();        \
                                        _RS_L();        \
                                        _DATA(val);     \
                                        _WR_L();        \
                                        _WR_H();        \
                                        _CS_H();        \
                                    } while (0)          

#define LCD_WRITE_DATA(val)         do {                \
                                        _CS_L();        \
                                        _RS_H();        \
                                        _DATA(val);     \
                                        _WR_L();        \
                                        _WR_H();        \
                                        _CS_H();        \
                                    } while (0)    
                                        
                                   
#define LCD_WRITE_REG(reg, data)    do {                       \
                                        LCD_WRITE_CMD(reg);    \
                                        LCD_WRITE_DATA(data);  \
                                    } while (0)

/* 
    internal function area
*/

static void _delay_ms(uint32 ms)
{
    ms = ms * 2500;
    while (ms--) {
        ;
    }
}

/*  
    func name:  lcd_io_init 
    input:      none
    output:     none
    note:       initialize all GPIO related to TFT control to a certain state
*/
static void _lcd_io_init(void)
{
    /* clock */
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
   
    /* PORT mux */
    PORTB_PCR2  = PORT_PCR_MUX(1);
    PORTB_PCR3  = PORT_PCR_MUX(1);
    PORTB_PCR16 = PORT_PCR_MUX(1);
    PORTB_PCR17 = PORT_PCR_MUX(1);
    PORTB_PCR18 = PORT_PCR_MUX(1);
    PORTB_PCR19 = PORT_PCR_MUX(1);
    PORTC_PCR0  = PORT_PCR_MUX(1);
    PORTC_PCR1  = PORT_PCR_MUX(1);
    PORTC_PCR2  = PORT_PCR_MUX(1);
    PORTC_PCR3  = PORT_PCR_MUX(1);
    PORTC_PCR4  = PORT_PCR_MUX(1);
    PORTC_PCR5  = PORT_PCR_MUX(1);
    PORTC_PCR6  = PORT_PCR_MUX(1);
    PORTC_PCR7  = PORT_PCR_MUX(1);
    PORTC_PCR8  = PORT_PCR_MUX(1);
    PORTC_PCR9  = PORT_PCR_MUX(1);
    PORTC_PCR10 = PORT_PCR_MUX(1);
    PORTC_PCR11 = PORT_PCR_MUX(1);
    PORTD_PCR0  = PORT_PCR_MUX(1);
    PORTD_PCR1  = PORT_PCR_MUX(1);
    PORTD_PCR2  = PORT_PCR_MUX(1);
    PORTD_PCR3  = PORT_PCR_MUX(1);
    
    /* Direction */
    FGPIOB_PDDR |= 0x000F000C; 
    FGPIOC_PDDR |= 0x00000FFF;
    FGPIOD_PDDR |= 0x0000000F;
    
    /* output high in default */
    FGPIOB_PSOR = 0x000F0004;   /* BLCNT drive LOW, other pins drive HIGH */
    FGPIOC_PSOR = 0x00000FFF;
    FGPIOD_PSOR = 0x00000FFF;
}

/*  
    func name: lcd_reset 
    input:none
    output:none
    note:reset lcd by GPIO
*/
static void _lcd_reset(void)
{
    _RST_L();
    _delay_ms(1);
    _RST_H();
    _delay_ms(120);
}

/* 
    external function area
*/

/*  
    func name:  lcd_backlight_ctrl 
    input:      ON/OFF
    output:     none
    note:       TO DO ctrl by PWM
*/
void lcd_backlight_ctrl(uint32 on_off)
{
    if (on_off == ON) {
        FGPIOB_PSOR = 1 << 3;
    } else {
        FGPIOB_PCOR = 1 << 3;
    }
}

/*  
    func name:  lcd_display_off 
    input:      none
    output:     none
    note:       turn off display
*/
void lcd_display_off(void)
{
    LCD_WRITE_CMD(0x28); 
}

/*  
    func name:  lcd_display_on 
    input:      none
    output:     none
    note:       turn on display
*/
void lcd_display_on(void)
{
    LCD_WRITE_CMD(0x29); 
}

/*  
    func name:  lcd_init 
    input:      none
    output:     none
    note:       initialize LCD and open display w/backlight.
*/
void lcd_init(void)
{
    _lcd_io_init();
    _lcd_reset();

    LCD_WRITE_CMD(0xCF);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x81);
    LCD_WRITE_DATA(0x30);

    LCD_WRITE_CMD(0xED);
    LCD_WRITE_DATA(0x64);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x81);

    LCD_WRITE_CMD(0xE8);
    LCD_WRITE_DATA(0x85);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x78);

    LCD_WRITE_CMD(0xCB);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x2C);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x34);
    LCD_WRITE_DATA(0x02);

    LCD_WRITE_CMD(0xF7);
    LCD_WRITE_DATA(0x20);

    LCD_WRITE_CMD(0xEA);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xB1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x1B);

    LCD_WRITE_CMD(0xB6);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0xA2);

    LCD_WRITE_CMD(0xC0);
    LCD_WRITE_DATA(0x35);

    LCD_WRITE_CMD(0xC1);
    LCD_WRITE_DATA(0x11);

    LCD_WRITE_CMD(0xC5);
    LCD_WRITE_DATA(0x45);
    LCD_WRITE_DATA(0x45);

    LCD_WRITE_CMD(0xC7);
    LCD_WRITE_DATA(0xA2);

    LCD_WRITE_CMD(0xF2);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0x26);
    LCD_WRITE_DATA(0x01);

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x26);
    LCD_WRITE_DATA(0x24);
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x0E);
    LCD_WRITE_DATA(0x09);
    LCD_WRITE_DATA(0x54);
    LCD_WRITE_DATA(0xA8);
    LCD_WRITE_DATA(0x46);
    LCD_WRITE_DATA(0x0C);
    LCD_WRITE_DATA(0x17);
    LCD_WRITE_DATA(0x09);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xE1); 
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x19);
    LCD_WRITE_DATA(0x1B);
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x07);
    LCD_WRITE_DATA(0x2A);
    LCD_WRITE_DATA(0x47);
    LCD_WRITE_DATA(0x39);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x30);
    LCD_WRITE_DATA(0x38);
    LCD_WRITE_DATA(0x0F);

    /* Bit7 - Bit 5 control the where (0, 0) located and how memory point increased automatically */
    LCD_WRITE_CMD(0x36); 
    LCD_WRITE_DATA(0x68); 

    LCD_WRITE_CMD(0x2A); 
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0xEF);

    LCD_WRITE_CMD(0x2B); 
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x3F);

    LCD_WRITE_CMD(0x3a);
    LCD_WRITE_DATA(0x55);
    
    LCD_WRITE_CMD(0x11);
    _delay_ms(120);
    LCD_WRITE_CMD(0x29);
    
    lcd_backlight_ctrl(ON);
}

/*  
    func name:  lcd_draw_pixel 
    input:      x, y, color
    output:     none
    note:       fill a point at (x, y) with color
*/
void lcd_draw_pixel(uint16 x, uint16 y, uint16 color)
{
    LCD_WRITE_CMD(0x2A); 
    LCD_WRITE_DATA(x >> 8);
    LCD_WRITE_DATA(x & 0xFF);

    LCD_WRITE_CMD(0x2B); 
    LCD_WRITE_DATA(y >> 8);
    LCD_WRITE_DATA(y & 0x0FF);
    
    LCD_WRITE_CMD(0x2C); 
    LCD_WRITE_DATA(color);
}

/*  
    func name:  lcd_draw_pixel 
    input:      x1, y1, x2, y2
    output:     none
    note:       set a WINDOW area for sequential frame memory fill
*/
void lcd_set_window(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
    LCD_WRITE_CMD(0x2A); 
    LCD_WRITE_DATA(x1 >> 8);
    LCD_WRITE_DATA(x1 & 0xFF);
    LCD_WRITE_DATA(x2 >> 8);
    LCD_WRITE_DATA(x2 & 0xFF);
    
    LCD_WRITE_CMD(0x2B); 
    LCD_WRITE_DATA(y1 >> 8);
    LCD_WRITE_DATA(y1 & 0x0FF);
    LCD_WRITE_DATA(y2 >> 8);
    LCD_WRITE_DATA(y2 & 0x0FF);

    LCD_WRITE_CMD(0x2C); 
}

/*  
    func name:  lcd_fill_window 
    input:      x1, y1, x2, y2, color
    output:     none
    note:       fill a window at (x, y) with color
*/
void lcd_fill_window(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color)
{
    uint32 cnt;
    uint32 total_pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    
    //lcd_display_off();
    
    lcd_set_window(x1, y1, x2, y2);

    for(cnt = 0; cnt < total_pixel_num; cnt++)
    {
        LCD_WRITE_DATA(color);
    }
    
    //lcd_display_on();
}








