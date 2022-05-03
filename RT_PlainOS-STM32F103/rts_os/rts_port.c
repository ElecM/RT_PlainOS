/**================================================================================================================
|
| 文件：rts_port.h
| 说明：RTS_OS硬件相关的底层接口（移植RTS操作系统主要修改rts_port.c和rts_port.h文件）
|       任务切换函数使用裸函数的原因是为了适应不同的编译优化选项，目前该版本支持到了-Os与-O2优化，
|       对于-O3的极端优化暂不支持。
| 作者：ElecM  (1508555728@qq.com)
| 开源协议：MIT
| 版本：v2.02
|
| 代码命名的规则：
| (1)宏全部大写，如果是关于OS内核的，则加上前缀RTS_
| (2)函数名使用大驼峰式命名规范
| (3)变量采用小写字母加下划线形式，OS相关的全局变量需要加上前缀rts_gb.
| (4)用户自定义类型使用小驼峰式命名规范，同时加上后缀_t
|
| 2021.11.16 by ElecM.
**==============================================================================================================**/


#include "rts_port.h"

//RTS OS滴答定时器初始化，移植时需要用户自己实现
void RTS_PORT_SystickInit(void)
{
    /** 填写你的OS滴答定时器初始化代码 **/
    SysTick_Config(0xefffffUL);
    RTS_SYSTICK_ENABLE
    RTS_DISABLE_INTERRUPTS  /*< 关中断 */
}


//任务堆栈初始化，移植需要用户自己实现
u32_t* RTS_PORT_TaskStackInit(void (*task)(void *data) ,
                                 u32_t     *task_stack,
                                 u16_t    stack_size)
{
    task_stack      +=  stack_size;
    *(--task_stack)  = (u32_t)(1<<24); //XPSR
    *(--task_stack)  = (u32_t)task;    //PC
    *(--task_stack)  = 0ul;   //R0
    *(--task_stack)  = 0ul;   //R1
    *(--task_stack)  = 0ul;   //R2
    *(--task_stack)  = 0ul;   //R3
    *(--task_stack)  = 0ul;   //R12
    *(--task_stack)  = 0ul;   //LR

    *(--task_stack)  = 0ul;   //R4
    *(--task_stack)  = 0ul;   //R5
    *(--task_stack)  = 0ul;   //R6
    *(--task_stack)  = 0ul;   //R7
    *(--task_stack)  = 0ul;   //R8
    *(--task_stack)  = 0ul;   //R9
    *(--task_stack)  = 0ul;   //R10
    *(--task_stack)  = 0ul;   //R11
    return task_stack;
}



