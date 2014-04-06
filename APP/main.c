#include "common.h"
#include "bsp.h"
#include "lcd.h"
#include "led.h"

static void AppTaskStart (void *p_arg);
static void AppTaskTimingCtrl (void *p_arg);
static void AppTaskExecutor (void *p_arg);

static OS_STK AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK AppTaskTimingCtrlStk[APP_CFG_TASK_TIMINGCTRL_STK_SIZE];
static OS_STK AppTaskExecutorStk[APP_CFG_TASK_EXECUTOR_STK_SIZE];

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
    lcd_init();

    lcd_fill_window(0, 0, LCD_XSIZE - 1, LCD_YSIZE - 1, GREY);
    lcd_draw_h_line(10, 2, 15, RED);
    lcd_draw_v_line(10, 10, 50, BLUE);
    
    pLEDctrlSemphore = OSSemCreate(0);
    
    OSTaskCreateExt((void (*)(void *)) AppTaskTimingCtrl,            /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskTimingCtrlStk[APP_CFG_TASK_TIMINGCTRL_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_TIMINGCTRL_PRIO,
                    (INT16U          ) APP_CFG_TASK_TIMINGCTRL_PRIO,
                    (OS_STK         *)&AppTaskTimingCtrlStk[0],
                    (INT32U          ) APP_CFG_TASK_TIMINGCTRL_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskCreateExt((void (*)(void *)) AppTaskExecutor,             /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskExecutorStk[APP_CFG_TASK_EXECUTOR_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_EXECUTOR_PRIO,
                    (INT16U          ) APP_CFG_TASK_EXECUTOR_PRIO,
                    (OS_STK         *)&AppTaskExecutorStk[0],
                    (INT32U          ) APP_CFG_TASK_EXECUTOR_STK_SIZE,
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
        
        led_toggle();
        
        if (cnt == 0) {
            cnt++;
            lcd_fill_window(50, 50, 250, 150, YELLOW);
        } else {
            cnt = 0;
            lcd_fill_window(50, 50, 250, 150, GREY);
        }
    }
}
