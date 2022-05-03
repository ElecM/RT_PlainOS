/**================================================================================================================
|
| 文件：rts_port.h
| 说明：RTS_OS硬件相关的底层接口（移植RTS操作系统主要修改rts_port.c和rts_port.h文件）
|       任务切换函数使用裸函数的原因是为了适应不同的编译优化选项，目前该版本支持到了-Os与-O2优化，
|       对于-O3的极端优化暂不支持。
| 作者：SillyMem  (1508555728@qq.com)
| 开源协议：MIT
| 版本：v2.02
|
| 代码命名的规则：
| (1)宏全部大写，如果是关于OS内核的，则加上前缀RTS_
| (2)函数名使用大驼峰式命名规范
| (3)变量采用小写字母加下划线形式，OS相关的全局变量需要加上前缀rts_gb.
| (4)用户自定义类型使用小驼峰式命名规范，同时加上后缀_t
|
| 2022.13.13 by SillyMem.
**==============================================================================================================**/


#include "rts_port.h"

//RTS OS滴答定时器初始化，移植时需要用户自己实现
void RTS_PORT_SystickInit(void)
{
    /** 填写你的OS滴答定时器初始化代码 **/
    TCCR1B = 0x02;   //进行8预分频  16MHz
    TCNT1H = 0X00;
    TCNT1L = 0X00;
    OCR1AH = 0XFF;
    OCR1AL = 0XFF;
    TIFR1  = 0X00;
    
    
    RTS_DISABLE_INTERRUPTS  /*< 关中断 */
}


//任务堆栈初始化，移植需要用户自己实现
u8_t* RTS_PORT_TaskStackInit(void (*task)(void *data) ,
                                 u8_t     *task_stack,
                                 u16_t    stack_size)
{
    task_stack       +=  stack_size-1;
    *(task_stack--)  =   (u8_t)((u16_t)task);
    *(task_stack--)  =   (u8_t)((u16_t)task>>8);
    *(task_stack--)  =   (u8_t)((u16_t)task>>16);
    *(task_stack--)  =   0u;   //r1
    *(task_stack--)  =   0u;   //r0
    *(task_stack--)  =   (u8_t)(1<<7);   //开中断标志，为了让每个任务都有可能被切换
    *(task_stack--)  =   0u;   //RAMP
    *(task_stack--)  =   0u;   //r2
    *(task_stack--)  =   0u;   //r3
    *(task_stack--)  =   0u;   //r4
    *(task_stack--)  =   0u;   //r5
    *(task_stack--)  =   0u;   //r6
    *(task_stack--)  =   0u;   //r7
    *(task_stack--)  =   0u;   //r8
    *(task_stack--)  =   0u;   //r9
    *(task_stack--)  =   0u;   //r10
    *(task_stack--)  =   0u;   //r11
    *(task_stack--)  =   0u;   //r12
    *(task_stack--)  =   0u;   //r13
    *(task_stack--)  =   0u;   //r14
    *(task_stack--)  =   0u;   //r15
    *(task_stack--)  =   0u;   //r16
    *(task_stack--)  =   0u;   //r17
    *(task_stack--)  =   0u;   //r18
    *(task_stack--)  =   0u;   //r19
    *(task_stack--)  =   0u;   //r20
    *(task_stack--)  =   0u;   //r21
    *(task_stack--)  =   0u;   //r22
    *(task_stack--)  =   0u;   //r23
    *(task_stack--)  =   0u;   //r24
    *(task_stack--)  =   0u;   //r25
    *(task_stack--)  =   0u;   //r26
    *(task_stack--)  =   0u;   //r27
    *(task_stack--)  =   0u;   //r30
    *(task_stack--)  =   0u;   //r31
    *(task_stack--)  =   0u;   //r29
    *(task_stack--)  =   0u;   //r28

    return task_stack;
}



