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

#ifndef __PORTS_H__
#define __PORTS_H__


#include <avr/io.h>
#include <avr/interrupt.h>
#include "datatype.h"

//* 进入临界区
#define RTS_ENTER_CRITICAL              asm volatile("lds	r0, 0x5f");\
                                        asm volatile("push	r0");\
                                        SREG &= (u8_t)(~(1<<7));

//* 退出临界区
#define RTS_EXIT_CRITICAL               asm volatile("pop	r0");\
                                        asm volatile("sts	0x5f, r0");

#define RTS_ENABLE_INTERRUPTS           SREG |= (u8_t)(1<<7);     /*< 开滴答定时器中断 */
#define RTS_DISABLE_INTERRUPTS          SREG &= (u8_t)(~(1<<7));  /*< 关滴答定时器中断 */

#define RTS_SYSTICK_HANDLER             ISR(TIMER1_OVF_vect)      /*< 滴答定时器中断向量 */

#define RTS_SYSTICK_HANDLER_ENTER_DO                              /*< 滴答定时器中断函数进入时的钩子 */

#define RTS_SYSTICK_CLEAR_INT_FLAG                                /*< 滴答定时器中断函数推出前清除中断标志位 */

#define RTS_SYSTICK_HANDLER_EXIT_DO     do{PORTB = ~ PORTB;}while(0); /*< 滴答定时器中断函数退出时的钩子 */

#define RTS_SYSTICK_ENABLE              TIMSK1 |= (u8_t)(1<<0);   /*< 启动滴答定时器 */ 

#define RTS_SYSTICK_DISABLE             TIMSK1 &= (u8_t)(~(1<<0));/*< 关闭滴答定时器 */


void  RTS_PORT_SystickInit(void);                                 /*< RTS系统滴答时钟初始化 */
u8_t* RTS_PORT_TaskStackInit(void (*task)(void *data),            /*< 任务堆栈初始化函数 */
                                 u8_t     *task_stack,
                                 u16_t    stack_size);


//extern  volatile void * volatile rts_gb_curr_task_tcb;          /* RTS当前TCB的全局指针,使用宏则无需进行声明 */
/********************************** 实现上下文切换的宏定义(内联汇编) ***********************************/

#define   RTS_PORT_TASK_SW_TOP     /* 上下文切换上部分(保存现场) */\
{\
   asm volatile("push   r1");        /*< R29入栈   */\
   asm volatile("push	r0");        /*< R28入栈   */\
\
   asm volatile("lds	   r0, 0x5f");      /*< SREG入栈  */\
   asm volatile("push	r0");\
   asm volatile("lds	r0, 0x5B");      /*< RAMPZ入栈 */\
   asm volatile("push	r0");\
\
   asm volatile("push	r2");\
   asm volatile("push	r3");\
   asm volatile("push	r4");\
   asm volatile("push	r5");\
   asm volatile("push	r6");\
   asm volatile("push	r7");\
   asm volatile("push	r8");\
   asm volatile("push	r9");\
   asm volatile("push	r10");\
   asm volatile("push	r11");\
   asm volatile("push	r12");\
   asm volatile("push	r13");\
   asm volatile("push	r14");\
   asm volatile("push	r15");\
   asm volatile("push	r16");\
   asm volatile("push	r17");\
   asm volatile("push	r18");\
   asm volatile("push	r19");\
   asm volatile("push	r20");\
   asm volatile("push	r21");\
   asm volatile("push	r22");\
   asm volatile("push	r23");\ 
\
   asm volatile("push   r24");        /*< R16入栈   */\
   asm volatile("push   r25");        /*< R16入栈   */\
   asm volatile("push	r26");        /*< R17入栈   */\
   asm volatile("push   r27");        /*< ...入栈   */\
   asm volatile("push	r30");\
   asm volatile("push   r31");\
   asm volatile("push	r29");\
   asm volatile("push   r28");\
   *((u16_t*)rts_gb_curr_task_tcb) = SP; /*< 保存正在运行任务的SP          */\
}



#define   RTS_PORT_TASK_SW_BOTTOM        /* 上下文切换下部分(恢复现场) */\
{\
   SP = *((u16_t*)rts_gb_curr_task_tcb); /*< 将SP指向下一个要运行的任务SP   */\
\
                                         /*< 清除滴答定时器的中断标志位     */\
   asm volatile("pop	  r28");            /*< 各寄存器出栈，恢复任务环境   */\
   asm volatile("pop   r29");\
   asm volatile("pop	  r31");\
   asm volatile("pop   r30");\
   asm volatile("pop	  r27");\
   asm volatile("pop   r26");\
   asm volatile("pop	  r25");\
   asm volatile("pop	  r24");\
   asm volatile("pop   r23");\
   asm volatile("pop   r22");\
   asm volatile("pop   r21");\
   asm volatile("pop   r20");\
   asm volatile("pop   r19");\
   asm volatile("pop   r18");\
   asm volatile("pop   r17");\
   asm volatile("pop   r16");\
   asm volatile("pop   r15");\
   asm volatile("pop   r14");\
   asm volatile("pop   r13");\
   asm volatile("pop   r12");\
   asm volatile("pop   r11");\
   asm volatile("pop   r10");\
   asm volatile("pop   r9");\
   asm volatile("pop   r8");\
   asm volatile("pop   r7");\
   asm volatile("pop   r6");\
   asm volatile("pop   r5");\
   asm volatile("pop   r4");\
   asm volatile("pop   r3");\
   asm volatile("pop   r2");\
\
   asm volatile("pop	  r0");\
   asm volatile("sts	  0x5B, r0"); /*< RAMPZ出栈    */\
\
   asm volatile("pop   r0");       /*< SREG出栈     */\
   asm volatile("sts   0x5f, r0");\
\
   asm volatile("pop   r0");\
   asm volatile("pop	  r1");\
   asm volatile("ret");             /*< PC出栈后跳转 */\
\
}


#endif // __PORTS_H__
