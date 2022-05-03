/**================================================================================================================
|
| 文件：rts_port.c
| 说明：本文件CPU相关源文件
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

#include "rts_port.h"

//RTS OS滴答定时器初始化，移植时需要用户自己实现
void RTS_PORT_SystickInit(void)
{
    /** 填写你的OS滴答定时器初始化代码 **/
    TCCR1B|=0X02;
    TCNT1=0xEFFF;
    RTS_DISABLE_INTERRUPTS  /*< 关中断 */
}


//任务堆栈初始化，移植需要用户自己实现
u8_t* RTS_PORT_TaskStackInit(void (*task)(void *data) ,
                                 u8_t     *task_stack,
                                 u16_t    stack_size)
{
    task_stack      +=  stack_size - 1;
    *(task_stack--)  = (u8_t)((u16_t)task);
    *(task_stack--)  = (u8_t)((u16_t)task>>8);
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;   
    *(task_stack--)  = 0u;
    *(task_stack--)  = (u8_t)(1<<7);  //开中断标志，为了让每个任务都有可能被切换
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    *(task_stack--)  = 0u;
    return task_stack;

}




