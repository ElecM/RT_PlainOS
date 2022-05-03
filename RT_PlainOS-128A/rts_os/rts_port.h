/**================================================================================================================
|
| 文件：rts_port.h
| 说明：本文件CPU相关头文件
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
#ifndef __RTS_PORTS_H__
#define __RTS_PORTS_H__


#include <avr/io.h>
#include <avr/interrupt.h>
#include "datatype.h"

//* 进入临界区
#define RTS_ENTER_CRITICAL              asm volatile("lds	r0, 0x5f");\
                                        asm volatile("push	r0");\
                                        SREG &= (u8_t)(~(1<<7));

//* 退出临界区
#define RTS_EXIT_CRITICAL               asm volatile("pop	r0");\
                                        asm volatile("sts	0x5f, r0");\


#define RTS_ENABLE_INTERRUPTS           SREG |= (u8_t)(1<<7);     /*< 开中断 */
#define RTS_DISABLE_INTERRUPTS          SREG &= (u8_t)(~(1<<7));  /*< 关中断 */

#define RTS_SYSTICK_ENABLE              TIMSK|=(u8_t)(1<<2);            /*< 开滴答定时器 */

#define RTS_SYSTICK_DISABLE             TIMSK&=(~(u8_t)(1<<2));         /*< 关滴答定时器 */

#define RTS_SYSTICK_HANDLER             ISR(TIMER1_OVF_vect)      /*< 滴答定时器中断向量 */

#define RTS_SYSTICK_HANDLER_ENTER_DO    /*< 滴答定时器中断函数进入时的钩子 */

#define RTS_SYSTICK_CLEAN_INT_FLAG      SREG &= (u8_t)(~(1<<7));\
                                        TCNT1=0xEfff;\
                                        SREG |= (u8_t)(1<<7);      /*< 滴答定时器中断函数退出前清除中断标志位 */

#define RTS_SYSTICK_HANDLER_EXIT_DO     //PORTC = ~PORTC;/*< 滴答定时器中断函数退出时的钩子 */


void  RTS_PORT_SystickInit(void);                                 /*< RTS系统滴答时钟初始化 */
u8_t* RTS_PORT_TaskStackInit(void (*task)(void *data)   ,         /*< 任务堆栈初始化函数 */
                                 u8_t     *task_stack,
                                 u16_t    stack_size);
                                 
void  RTS_PORT_SwitchToNextTask(void);                /*< 上下文切换函数，见rts_port_asm.S */




#endif // __PORTS_H__
