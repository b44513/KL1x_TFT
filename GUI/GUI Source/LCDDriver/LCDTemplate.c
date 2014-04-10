/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : LCDTemplate.c
Purpose     : Empty driver
              This driver does no perform any function, but it can be
              used for 2 purposes:
              a) Satisfy all externals so an application can be
                compiled and linked in target hardware even if the
                driver is not already available
              b) Template for a starting point for a new driver.
----------------------------------------------------------------------   
Adapting to a new system (creating a new driver):
  In this case, the first step is to fill the routines 
  LCD_L0_GetPixelIndex, LCD_L0_SetPixelIndex and LCD_L0_Init with
  functionality, which is sufficient to make the hardware work.
  A second (optional) step would be to optimize higher level routines. 
----------------------------------------------------------------------   
Version-Date---Author-Explanation                                        
----------------------------------------------------------------------   
1.00.00 020417 JE     a) Changed to have only to adapt _GetPixelIndex
                         and _SetPixelIndex
0.90.00 020214 JE     a) First release
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include "LCD_Private.h"      /* private modul definitions & config */
#include "GUI_Private.h"
#include "GUIDebug.h"
#include <ucos_ii.h>
#include "common.h"

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

#define _WR_DATA(val)               do {                   \
                                        FGPIOC_PDOR = val; \
                                        FGPIOD_PDOR = (val) >> 12;  \
                                    } while (0)            \
										
#define _RD_DATA(p_val)             do {                   \
                                        *(p_val) = (FGPIOC_PDIR & 0x00000FFF);			\
                                        *(p_val) |= ((FGPIOD_PDIR & 0x0000000F) << 12);	\
                                    } while (0)            \
                                        
#define LCD_WRITE_CMD(val)          do {                \
                                        _CS_L();        \
                                        _RS_L();        \
                                        _WR_L();        \
                                        _WR_DATA(val);	\
                                        _WR_H();        \
                                        _CS_H();        \
                                    } while (0)          

#define LCD_WRITE_DATA(val)         do {                \
                                        _CS_L();        \
                                        _RS_H();        \
                                        _WR_L();        \
                                        _WR_DATA(val);	\
                                        _WR_H();        \
                                        _CS_H();        \
                                    } while (0)    
										
#define LCD_CFG_DATA_RD()			do {                \
										FGPIOC_PDDR &= ~0x00000FFF;	\
										FGPIOD_PDDR &= ~0x0000000F;	\
                                    } while (0)    
										
#define LCD_CFG_DATA_WR()			do {                \
										FGPIOC_PDDR |= 0x00000FFF;	\
										FGPIOD_PDDR |= 0x0000000F;	\
										FGPIOC_PSOR = 0x00000FFF;	\
										FGPIOD_PSOR = 0x0000000F;	\
                                    } while (0)    

/* Tra must > 340ns, a nop is ~20ns @ 48MHz */
#define _DELAY_TRA()                do {                    \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                    } while (0)             \
                                        
/* Trdh must > 90ns, a nop is ~20ns @ 48MHz */
#define _DELAY_TRDH()               do {                    \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                        asm("nop");         \
                                     } while (0)            \
                                            
#define LCD_READ_DATA(p_val)         do {					\
                                        _CS_L();			\
                                        _RS_H();			\
                                        _RD_L();			\
                                        _DELAY_TRA();       \
										_RD_DATA(p_val);	\
                                        _RD_H();			\
                                        _DELAY_TRDH();      \
                                        _CS_H();        	\
                                    } while (0)    

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#ifndef   LCD_INIT_CONTROLLER
  #define LCD_INIT_CONTROLLER()
#endif
#ifndef   LCD_SET_ORG
  #define LCD_SET_ORG(x, y) GUI_USE_PARA(x); GUI_USE_PARA(y)
#endif

/*********************************************************************
*
*       Macros for MIRROR_, SWAP_ and LUT_
*/
#if (!defined (LCD_LUT_COM) && !defined(LCD_LUT_SEG))
  #if   (!LCD_MIRROR_X && !LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) y
  #elif (!LCD_MIRROR_X && !LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) y
    #define LOG2PHYS_Y(x, y) x
  #elif (!LCD_MIRROR_X &&  LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) LCD_YSIZE - 1 - (y)
  #elif (!LCD_MIRROR_X &&  LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) y
    #define LOG2PHYS_Y(x, y) LCD_XSIZE - 1 - (x)
  #elif ( LCD_MIRROR_X && !LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_XSIZE - 1 - (x)
    #define LOG2PHYS_Y(x, y) y
  #elif ( LCD_MIRROR_X && !LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_YSIZE - 1 - (y)
    #define LOG2PHYS_Y(x, y) x
  #elif ( LCD_MIRROR_X &&  LCD_MIRROR_Y && !LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_XSIZE - 1 - (x)
    #define LOG2PHYS_Y(x, y) LCD_YSIZE - 1 - (y)
  #elif ( LCD_MIRROR_X &&  LCD_MIRROR_Y &&  LCD_SWAP_XY) 
    #define LOG2PHYS_X(x, y) LCD_YSIZE - 1 - (y)
    #define LOG2PHYS_Y(x, y) LCD_XSIZE - 1 - (x)
  #endif
#else
  #if   ( defined (LCD_LUT_COM) && !defined(LCD_LUT_SEG))
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) LCD__aLine2Com0[y]
  #elif (!defined (LCD_LUT_COM) &&  defined(LCD_LUT_SEG))
    #define LOG2PHYS_X(x, y) LCD__aCol2Seg0[x]
    #define LOG2PHYS_Y(x, y) y
  #elif ( defined (LCD_LUT_COM) &&  defined(LCD_LUT_SEG))
    #define LOG2PHYS_X(x, y) LCD__aCol2Seg0[x]
    #define LOG2PHYS_Y(x, y) LCD__aLine2Com0[y]
  #endif
#endif

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/

/*********************************************************************
*
*       Draw Bitmap 1 BPP
*/
static void  _DrawBitLine1BPP(int x, int y, U8 const GUI_UNI_PTR *p, int Diff, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX Index0 = *(pTrans+0);
  LCD_PIXELINDEX Index1 = *(pTrans+1);
  x += Diff;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
      do {
        LCD_L0_SetPixelIndex(x++, y, (*p & (0x80 >> Diff)) ? Index1 : Index0);
			  if (++Diff == 8) {
          Diff = 0;
				  p++;
			  }
		  } while (--xsize);
      break;
    case LCD_DRAWMODE_TRANS:
      do {
  		  if (*p & (0x80 >> Diff))
          LCD_L0_SetPixelIndex(x, y, Index1);
        x++;
			  if (++Diff == 8) {
          Diff = 0;
				  p++;
			  }
		  } while (--xsize);
      break;
    case LCD_DRAWMODE_XOR | LCD_DRAWMODE_TRANS:
    case LCD_DRAWMODE_XOR:
      do {
  		  if (*p & (0x80 >> Diff)) {
          int Pixel = LCD_L0_GetPixelIndex(x, y);
          LCD_L0_SetPixelIndex(x, y, LCD_NUM_COLORS - 1 - Pixel);
        }
        x++;
			  if (++Diff == 8) {
          Diff = 0;
				  p++;
			  }
		  } while (--xsize);
      break;
	}
}

/*********************************************************************
*
*       Draw Bitmap 2 BPP
*/
#if (LCD_MAX_LOG_COLORS > 2)
static void  _DrawBitLine2BPP(int x, int y, U8 const GUI_UNI_PTR * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixels = *p;
  int CurrentPixel = Diff;
  x += Diff;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
      if (pTrans) {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
          LCD_L0_SetPixelIndex(x++, y, PixelIndex);
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          LCD_L0_SetPixelIndex(x++, y, Index);
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
    case LCD_DRAWMODE_TRANS:
      if (pTrans) {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          if (Index) {
            LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
            LCD_L0_SetPixelIndex(x, y, PixelIndex);
          }
          x++;
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (3 - CurrentPixel) << 1;
          int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
          if (Index) {
            LCD_L0_SetPixelIndex(x, y, Index);
          }
          x++;
          if (++CurrentPixel == 4) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
  }
}
#endif

/*********************************************************************
*
*       Draw Bitmap 4 BPP
*/
#if (LCD_MAX_LOG_COLORS > 4)
static void  _DrawBitLine4BPP(int x, int y, U8 const GUI_UNI_PTR * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixels = *p;
  int CurrentPixel = Diff;
  x += Diff;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
      if (pTrans) {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
          LCD_L0_SetPixelIndex(x++, y, PixelIndex);
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          LCD_L0_SetPixelIndex(x++, y, Index);
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
    case LCD_DRAWMODE_TRANS:
      if (pTrans) {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          if (Index) {
            LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
            LCD_L0_SetPixelIndex(x, y, PixelIndex);
          }
          x++;
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      } else {
        do {
          int Shift = (1 - CurrentPixel) << 2;
          int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
          if (Index) {
            LCD_L0_SetPixelIndex(x, y, Index);
          }
          x++;
          if (++CurrentPixel == 2) {
            CurrentPixel = 0;
            Pixels = *(++p);
          }
		    } while (--xsize);
      }
      break;
  }
}
#endif

/*********************************************************************
*
*       Draw Bitmap 8 BPP
*/
#if (LCD_MAX_LOG_COLORS > 16)
static void  _DrawBitLine8BPP(int x, int y, U8 const GUI_UNI_PTR * p, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixel;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
    case 0:
      if (pTrans) {
        for (; xsize > 0; xsize--, x++, p++) {
          Pixel = *p;
          LCD_L0_SetPixelIndex(x, y, *(pTrans + Pixel));
        }
      } else {
        for (; xsize > 0; xsize--, x++, p++) {
          LCD_L0_SetPixelIndex(x, y, *p);
        }
      }
      break;
    case LCD_DRAWMODE_TRANS:
      if (pTrans) {
        for (; xsize > 0; xsize--, x++, p++) {
          Pixel = *p;
          if (Pixel) {
            LCD_L0_SetPixelIndex(x, y, *(pTrans + Pixel));
          }
        }
      } else {
        for (; xsize > 0; xsize--, x++, p++) {
          Pixel = *p;
          if (Pixel) {
            LCD_L0_SetPixelIndex(x, y, Pixel);
          }
        }
      }
      break;
  }
}
#endif

/*********************************************************************
*
*       Draw Bitmap 16 BPP
*/
#if (LCD_BITSPERPIXEL > 8)
static void  DrawBitLine16BPP(int x, int y, U16 const GUI_UNI_PTR * p, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX pixel;
  if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) == 0) {
    if (pTrans) {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        LCD_L0_SetPixelIndex(x, y, *(pTrans + pixel));
      }
    } else {
      for (;xsize > 0; xsize--, x++, p++) {
        LCD_L0_SetPixelIndex(x, y, *p);
      }
    }
  } else {
    if (pTrans) {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          LCD_L0_SetPixelIndex(x, y, *(pTrans + pixel));
        }
      }
    } else {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          LCD_L0_SetPixelIndex(x, y, pixel);
        }
      }
    }
  }
}
#endif

/*********************************************************************
*
*       Exported functions
*
**********************************************************************
*/

/*********************************************************************
*
*       LCD_L0_SetPixelIndex
*
* Purpose:
*   Sets the index of the given pixel. The upper layers
*   calling this routine make sure that the coordinates are in range, so
*   that no check on the parameters needs to be performed.
*/
void LCD_L0_SetPixelIndex(int x, int y, int PixelIndex) {
  /* Convert logical into physical coordinates (Dep. on LCDConf.h) */
  #if LCD_SWAP_XY | LCD_MIRROR_X| LCD_MIRROR_Y
    int xPhys = LOG2PHYS_X(x, y);
    int yPhys = LOG2PHYS_Y(x, y);
  #else
    #define xPhys x
    #define yPhys y
  #endif
  /* Write into hardware ... Adapt to your system */
    LCD_WRITE_CMD(0x2A); 
    LCD_WRITE_DATA(xPhys >> 8);
    LCD_WRITE_DATA(xPhys & 0xFF);

    LCD_WRITE_CMD(0x2B); 
    LCD_WRITE_DATA(yPhys >> 8);
    LCD_WRITE_DATA(yPhys & 0x0FF);

    LCD_WRITE_CMD(0x2C); 
    LCD_WRITE_DATA(PixelIndex);
}

/*********************************************************************
*
*       LCD_L0_GetPixelIndex
*
* Purpose:
*   Returns the index of the given pixel. The upper layers
*   calling this routine make sure that the coordinates are in range, so
*   that no check on the parameters needs to be performed.
*/
unsigned int LCD_L0_GetPixelIndex(int x, int y) {
  LCD_PIXELINDEX PixelIndex;
  /* Convert logical into physical coordinates (Dep. on LCDConf.h) */
  #if LCD_SWAP_XY | LCD_MIRROR_X| LCD_MIRROR_Y
    int xPhys = LOG2PHYS_X(x, y);
    int yPhys = LOG2PHYS_Y(x, y);
  #else
    #define xPhys x
    #define yPhys y
  #endif
  /* Read from hardware ... Adapt to your system */
    LCD_WRITE_CMD(0x2A); 
    LCD_WRITE_DATA(xPhys >> 8);
    LCD_WRITE_DATA(xPhys & 0xFF);

    LCD_WRITE_CMD(0x2B); 
    LCD_WRITE_DATA(yPhys >> 8);
    LCD_WRITE_DATA(yPhys & 0x0FF);

    LCD_WRITE_CMD(0x2E);

    LCD_CFG_DATA_RD();

    LCD_READ_DATA(&PixelIndex);
    LCD_READ_DATA(&PixelIndex);

    LCD_CFG_DATA_WR();

    return PixelIndex;
}

/*********************************************************************
*
*       LCD_L0_XorPixel
*/
void LCD_L0_XorPixel(int x, int y) {
  LCD_PIXELINDEX PixelIndex = LCD_L0_GetPixelIndex(x, y);
  LCD_L0_SetPixelIndex(x, y, LCD_NUM_COLORS - PixelIndex - 1);
}

/*********************************************************************
*
*       LCD_L0_DrawHLine
*/
void LCD_L0_DrawHLine  (int x0, int y,  int x1) {
#if LCD_SWAP_XY | LCD_MIRROR_X| LCD_MIRROR_Y
        int x_phys  = LOG2PHYS_X(x0, y);
        int y0_phys = LOG2PHYS_Y(x0, y);
        int y1_phys = LOG2PHYS_Y(x1, y);
#else
    #define x0_phys x0
    #define y_phys y
    #define x1_phys x1
#endif
    
  if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR) {
    for (; x0_phys <= x1_phys; x0_phys++) {
      LCD_L0_XorPixel(x0_phys, y_phys);
    }
  } else {
    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA(x0_phys >> 8);
    LCD_WRITE_DATA(x0_phys & 0xFF);

    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA(y_phys >> 8);
    LCD_WRITE_DATA(y_phys & 0x0FF);
    
    LCD_WRITE_CMD(0x2C);
      
    for (; x0_phys <= x1_phys; x0_phys++) {
        LCD_WRITE_DATA(LCD_COLORINDEX);
    }
  }
}

/*********************************************************************
*
*       LCD_L0_DrawVLine
*/
void LCD_L0_DrawVLine  (int x, int y0,  int y1) {
  if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR) {
    for (; y0 <= y1; y0++) {
      LCD_L0_XorPixel(x, y0);
    }
  } else {
    LCD_WRITE_CMD(0x36);
    LCD_WRITE_DATA(0x08);               /* (0, 0) at left top, Vertical Screen */
	
    /* must do coordinate conversion, physical (0, 0) is at left top */
#define TMPT_LOG2PHYS_X(x, y) LCD_YSIZE - 1 - (y)
#define TMPT_LOG2PHYS_Y(x, y) x
    
	uint16 y_phys  = TMPT_LOG2PHYS_Y(x, y0);
	uint16 x0_phys = TMPT_LOG2PHYS_X(x, y1);
	uint16 x1_phys = TMPT_LOG2PHYS_X(x, y0);
    
    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA(x0_phys >> 8);
    LCD_WRITE_DATA(x0_phys & 0xFF);

    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA(y_phys >> 8);
    LCD_WRITE_DATA(y_phys & 0x0FF);
    
    LCD_WRITE_CMD(0x2C);
    for (; x0_phys <= x1_phys; x0_phys++)
    {
        LCD_WRITE_DATA(LCD_COLORINDEX);
    }
    
    LCD_WRITE_CMD(0x36); 
    LCD_WRITE_DATA(0x68);               /* (0, 0) at right top, Horizontal Screen */
  }
}

/*********************************************************************
*
*       LCD_L0_FillRect
*/
void LCD_L0_FillRect(int x0, int y0, int x1, int y1) {
    if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)             
    {
        for (; x0 <= x1; x0++)
        {
            LCD_L0_DrawVLine(x0, y0, y1);
        }
    }
    else                                                      
    {

#if LCD_SWAP_XY | LCD_MIRROR_X | LCD_MIRROR_Y                
        int x0_phys = LOG2PHYS_X(x0, y1);
        int y0_phys = LOG2PHYS_Y(x0, y0);
        int x1_phys = LOG2PHYS_X(x1, y0);
        int y1_phys = LOG2PHYS_Y(x1, y1);
#else
        #define x0_phys x0
        #define y0_phys y0
        #define x1_phys x1
        #define y1_phys y1
#endif

        uint32 cnt;
        uint32 total_pixel_num = (x1_phys - x0_phys + 1) * (y1_phys - y0_phys + 1);

        LCD_WRITE_CMD(0x2A); 
        LCD_WRITE_DATA(x0_phys >> 8);
        LCD_WRITE_DATA(x0_phys & 0xFF);
        LCD_WRITE_DATA(x1_phys >> 8);
        LCD_WRITE_DATA(x1_phys & 0xFF);

        LCD_WRITE_CMD(0x2B); 
        LCD_WRITE_DATA(y0_phys >> 8);
        LCD_WRITE_DATA(y0_phys & 0x0FF);
        LCD_WRITE_DATA(y1_phys >> 8);
        LCD_WRITE_DATA(y1_phys & 0x0FF);

        LCD_WRITE_CMD(0x2C); 

        _CS_L();
        _RS_H();

        for(cnt = 0; cnt < total_pixel_num; cnt++)
        {
            _WR_L();
            _WR_DATA(LCD_COLORINDEX);
            _WR_H();
        }

        _CS_H();
    }
}

/*********************************************************************
*
*       LCD_L0_DrawBitmap
*/
void LCD_L0_DrawBitmap(int x0, int y0,
                       int xsize, int ysize,
                       int BitsPerPixel, 
                       int BytesPerLine,
                       const U8 GUI_UNI_PTR * pData, int Diff,
                       const LCD_PIXELINDEX* pTrans)
{
  int i;
  /* Use _DrawBitLineXBPP */
  for (i=0; i<ysize; i++) {
    switch (BitsPerPixel) {
    case 1:
      _DrawBitLine1BPP(x0, i + y0, pData, Diff, xsize, pTrans);
      break;
    #if (LCD_MAX_LOG_COLORS > 2)
      case 2:
        _DrawBitLine2BPP(x0, i + y0, pData, Diff, xsize, pTrans);
        break;
    #endif
    #if (LCD_MAX_LOG_COLORS > 4)
      case 4:
        _DrawBitLine4BPP(x0, i + y0, pData, Diff, xsize, pTrans);
        break;
    #endif
    #if (LCD_MAX_LOG_COLORS > 16)
      case 8:
        _DrawBitLine8BPP(x0, i + y0, pData, xsize, pTrans);
        break;
    #endif
    #if (LCD_BITSPERPIXEL > 8)
      case 16:
        DrawBitLine16BPP(x0, i + y0, (const U16 *)pData, xsize, pTrans);
        break;
    #endif
    }
    pData += BytesPerLine;
  }
}

/*********************************************************************
*
*       LCD_L0_SetOrg
*/
void LCD_L0_SetOrg(int x, int y) {
  LCD_SET_ORG(x, y);
}

/*********************************************************************
*
*       LCD_On / LCD_Off
*/
void LCD_On (void) {
#ifdef LCD_ON
  LCD_ON();
#endif
}

void LCD_Off (void) {
#ifdef LCD_OFF
  LCD_OFF();
#endif
}

/*********************************************************************
*
*       LCD_L0_Init
*
* Purpose:
*   Initialises the LCD-controller.
*/
int  LCD_L0_Init(void) {
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
    LCD_WRITE_DATA(0x68);       /* (0, 0) at right top, horizontal screen */
    //LCD_WRITE_DATA(0xA8);       /* (0, 0) at left bottom, horizontal screen */
    //LCD_WRITE_DATA(0x08);       /* (0, 0) at left top, vertical screen */
    //LCD_WRITE_DATA(0xC8);       /* (0, 0) at right bottom, vertical screen */
    
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
    OSTimeDlyHMSM(0, 0, 0, 120);       /* Delay 50 ms */
    LCD_WRITE_CMD(0x29);

    return 0;
}

/*********************************************************************
*
*       LCD_L0_SetLUTEntry
*/
void LCD_L0_SetLUTEntry(U8 Pos, LCD_COLOR Color) {
  GUI_USE_PARA(Pos);
  GUI_USE_PARA(Color);
}

/*********************************************************************
*
*       LCD_L0_GetDevFunc
*/
void * LCD_L0_GetDevFunc(int Index) {
  GUI_USE_PARA(Index);
  return NULL;
}

	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
