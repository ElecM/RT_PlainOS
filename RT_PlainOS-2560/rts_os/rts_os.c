#include "rts_os.h"


//RTS_OS初始化
void RTS_OS_Init(void (*idle_task_hook)(void *))
{
    RTS_DISABLE_INTERRUPTS
    MemAllocInit();
    RTS_PORT_SystickInit();
    RTS_CORE_Init();
    RTS_TASK_IdleTaskInit(idle_task_hook);
    rts_gb_systicks = 0;
}


//RTS_OS启动
void RTS_OS_Start(void)
{
    RTS_CORE_UpdateTaskHightPrio();
    RTS_CORE_GetHightPrioTcb((TCB_t **)(&rts_gb_curr_task_tcb));

    #if (RTS_CFG_DELAY_ENB == 0u)
    if(RTS_CORE_GetHiprioTcbNum() > 1u)
        RTS_SYSTICK_ENABLE
    else
        RTS_SYSTICK_DISABLE
    #endif
    
    RTS_PORT_TASK_SW_BOTTOM 
    RTS_ENABLE_INTERRUPTS  //开启总中断
}