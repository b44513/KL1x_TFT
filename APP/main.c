#include "common.h"
#include "bsp.h"
#include "lcd_9341.h"
#include "led.h"
#include <GUI.h>
#include <WM.h>

static void AppTaskStart (void *p_arg);
static void AppTaskTimingCtrl (void *p_arg);
static void AppTaskExecutor (void *p_arg);
static void AppTaskGUI (void *p_arg);

static OS_STK AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK AppTaskTimingCtrlStk[APP_CFG_TASK_TIMINGCTRL_STK_SIZE];
static OS_STK AppTaskExecutorStk[APP_CFG_TASK_EXECUTOR_STK_SIZE];
static OS_STK AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE];

static void __BackgroundWindowCallback (WM_MESSAGE *p_msg);

OS_EVENT *pLEDctrlSemphore;

int main (void)
{
#if (OS_TASK_NAME_EN > 0)
    INT8U   os_err;
#endif

    BSP_Init();

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"          */

    OSTaskCreateExt((void (*)(void *)) AppTaskStart,            /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&AppTaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet((INT8U     )APP_CFG_TASK_START_PRIO,
                  (INT8U    *)"Start_Task",
                  (INT8U    *)&os_err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    return (1);
}
__root uint16 testcolor;
static void AppTaskStart (void *p_arg)
{
    INT32U  cpu_clk_freq;
    INT32U  cnts;

    (void)p_arg;                                                /* Note #1                                             */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (INT32U)OS_TICKS_PER_SEC;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif
    
    led_init();
    /* lcd_io_init();
    lcd_reset();
    lcd_init();
    lcd_backlight_ctrl(ON); 

    lcd_fill_window(0, 0, LCD_XSIZE - 1, LCD_YSIZE - 1, GREY);
    testcolor = lcd_get_pixel(5, 5);
    lcd_draw_h_line(10, 2, 15, RED);
    lcd_draw_v_line(10, 10, 50, BLUE);*/
    
    pLEDctrlSemphore = OSSemCreate(0);
    
    OSTaskCreateExt((void (*)(void *)) AppTaskTimingCtrl,
                    (void           *) 0,
                    (OS_STK         *)&AppTaskTimingCtrlStk[APP_CFG_TASK_TIMINGCTRL_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_TIMINGCTRL_PRIO,
                    (INT16U          ) APP_CFG_TASK_TIMINGCTRL_PRIO,
                    (OS_STK         *)&AppTaskTimingCtrlStk[0],
                    (INT32U          ) APP_CFG_TASK_TIMINGCTRL_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskCreateExt((void (*)(void *)) AppTaskExecutor,
                    (void           *) 0,
                    (OS_STK         *)&AppTaskExecutorStk[APP_CFG_TASK_EXECUTOR_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_EXECUTOR_PRIO,
                    (INT16U          ) APP_CFG_TASK_EXECUTOR_PRIO,
                    (OS_STK         *)&AppTaskExecutorStk[0],
                    (INT32U          ) APP_CFG_TASK_EXECUTOR_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    
    OSTaskCreateExt((void (*)(void *)) AppTaskGUI,
                    (void           *) 0,
                    (OS_STK         *)&AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_GUI_PRIO,
                    (INT16U          ) APP_CFG_TASK_GUI_PRIO,
                    (OS_STK         *)&AppTaskGUIStk[0],
                    (INT32U          ) APP_CFG_TASK_GUI_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    while (OS_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0, 0, 5, 0);
    }
}

static void AppTaskTimingCtrl (void *p_arg)
{
    (void)p_arg;

    while (OS_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        (void)OSSemPost(pLEDctrlSemphore);
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

static void AppTaskExecutor (void *p_arg)
{
    INT8U cnt = 0;
    INT8U err;
    (void)p_arg;

    while (OS_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSSemPend(pLEDctrlSemphore, 0, &err);
        
        if (cnt == 0) {
            cnt++;
            led_ctrl(ON);
            //lcd_fill_window_fast(0, 0, LCD_XSIZE - 1, LCD_YSIZE - 1, RED);
            led_ctrl(OFF);
        } else {
            cnt = 0;
            led_ctrl(ON);
            //lcd_fill_window_fast(0, 0, LCD_XSIZE - 1, LCD_YSIZE - 1, YELLOW);
            led_ctrl(OFF);
        }
    }
}

static void AppTaskGUI (void *p_arg)
{
    (void)p_arg;

    GUI_Init();                                                                 /* 初始化GUI */

    WM_SetCallback(WM_HBKWIN, __BackgroundWindowCallback);
    
    lcd_backlight_ctrl(ON);

    while (OS_TRUE) {
        GUI_Exec();
    }
}

static void __BackgroundWindowCallback (WM_MESSAGE *p_msg)
{
	switch (p_msg->MsgId)
	{
    case WM_KEY:
        {
			;
        }
        break;
		
	case WM_PAINT:
		{
			GUI_SetBkColor(GUI_BLUE);
			GUI_Clear();
            GUI_SetColor(GUI_RED);
            GUI_SetFont(&GUI_FontComic24B_ASCII);
            GUI_DispStringAt("Test uC/GUI", 100, 10);
            
            GUI_SetBkColor(GUI_BLACK);
            GUI_SetColor(GUI_YELLOW);
            GUI_FillRect(50, 50, 100, 70);
        }
		break;
		
	default:                                                                    /* 不接收的消息抛给默认处理 */
		{
			WM_DefaultProc(p_msg);
		}
		break;
	}
}

