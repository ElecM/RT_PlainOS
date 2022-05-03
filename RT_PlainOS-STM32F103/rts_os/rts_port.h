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
#ifndef __RTS_PORTS_H__
#define __RTS_PORTS_H__

#include "datatype.h"
#include "stm32f10x.h"


//* 进入临界区
#define RTS_ENTER_CRITICAL             RTS_DISABLE_INTERRUPTS

                                       //__asm__ volatile("mrs r6,	primask\n\t");
                                       // __asm__ volatile("push  {r6}\n\t");
                                       // __asm__ volatile("cpsid	i\n\t");

//* 退出临界区
#define RTS_EXIT_CRITICAL              RTS_ENABLE_INTERRUPTS
                                       //__asm__ volatile("pop {r6}\n\t");
                                       //__asm__ volatile("msr primask,r6\n\t");


#define RTS_ENABLE_INTERRUPTS           __asm__ volatile("cpsie	i\n\t");     /*< 开中断 */
#define RTS_DISABLE_INTERRUPTS          __asm__ volatile("cpsid	i\n\t");     /*< 关中断 */


#define RTS_SYSTICK_ENABLE              __NVIC_EnableIRQ(SysTick_IRQn);      /*< 开滴答定时器 */

#define RTS_SYSTICK_DISABLE             __NVIC_DisableIRQ(SysTick_IRQn);     /*< 关滴答定时器 */

#define RTS_SYSTICK_HANDLER             void SysTick_Handler(void)           /*< 滴答定时器中断向量 */

#define RTS_TRIGGER_PENDSV              do{SCB->ICSR |= (1<<28);}while(0);   /*< 挂起PendSV */
#define RTS_CLEAR_PENDSV                do{SCB->ICSR |= (1<<27);}while(0);   /*< 清除PendSV */


#define RTS_SYSTICK_HANDLER_ENTER_DO    /*< 滴答定时器中断函数进入时的钩子 */

#define RTS_SYSTICK_CLEAN_INT_FLAG      /*< 滴答定时器中断函数退出前清除中断标志位 */

#define RTS_SYSTICK_HANDLER_EXIT_DO     //PORTC = ~PORTC;/*< 滴答定时器中断函数退出时的钩子 */


void  RTS_PORT_SystickInit(void);                                 /*< RTS系统滴答时钟初始化 */
u32_t* RTS_PORT_TaskStackInit(void (*task)(void *data)   ,         /*< 任务堆栈初始化函数 */
                                 u32_t     *task_stack,
                                 u16_t     stack_size);

void RTS_PORT_SwitchToNextTask(void);


#endif // __PORTS_H__
