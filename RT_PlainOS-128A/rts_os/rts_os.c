/**================================================================================================================
|
| 文件：rts_os.c
| 说明：本文件RTS OS 内核源文件
|       
| 作者：SillyMem  (1508555728@qq.com)
| 开源协议：MIT
| 版本：v2.24
|
| 代码命名的规则：
| (1)宏全部大写，如果是关于OS内核的，则加上前缀RTS_
| (2)函数名使用大驼峰式命名规范
| (3)变量采用小写字母加下划线形式，OS相关的全局变量需要加上前缀rts_gb.
| (4)用户自定义类型使用小驼峰式命名规范，同时加上后缀_t
|
| 2022.04.13 by SillyMem.
**==============================================================================================================**/

#include "rts_os.h"

//RTS_OS初始化
void RTS_OS_Init(void (*idle_task_hook)(void *))
{
    RTS_DISABLE_INTERRUPTS
    MemAllocInit();
    RTS_CORE_Init();
    RTS_TASK_IdleTaskInit(idle_task_hook);
    rts_gb_systicks = 0;
}


//RTS_OS启动
void RTS_OS_Start(void)
{
    RTS_CORE_UpdateTaskHightPrio();
    RTS_CORE_GetHightPrioTcb((TCB_t **)(&rts_gb_curr_task_tcb));
    rts_gb_prev_task_tcb = rts_gb_curr_task_tcb;
    #if (RTS_CFG_DELAY_ENB == 0u)
    if(RTS_CORE_GetHiprioTcbNum() > 1u)
        RTS_SYSTICK_ENABLE
    else
        RTS_SYSTICK_DISABLE
    #endif
    RTS_PORT_SystickInit();
    RTS_ENABLE_INTERRUPTS  //开启总中断
    RTS_SYSTICK_ENABLE
    RTS_PORT_SwitchToNextTask(); 
}