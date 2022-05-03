/*===============================================================================================
 文件：rts_port.S
 说明：任务切换相关函数
 作者：lishanwu  (SillyMem)
 时间：2022.04.09
 附言：该文件针对STM32F103的RTS操作系统移植

===============================================================================================*/
#define  SCB_ICSR_REG   0xE000ED04
#define  GPIOD_ODR_REG  0x4001140C

.extern rts_gb_prev_task_tcb   /*< 当前任务TCB指针 */
.extern rts_gb_curr_task_tcb   /*< 下一任务TCB指针 */


.global PendSV_Handler
.global RTS_PORT_TriggerPendsv



  .section .data.once_entry
  .type once_entry, %object
once_entry:
  .word  0x00

/*
在Thumb模式下，push/pop是单独的指令，具有自己的助记符和操作码。e.g. b500 push {lr}。
使用它是因为它更高效（当您的寄存器列表只包括r0..r7，lr中的寄存器时）
stmfd存在于Thumb-2中，但GAS显然只能在.syntax unified模式下正确处理它。
如果没有这一点，stmfd sp!,{lr}将给出一个Error: lo register required。
但是有了.syntax unified我们得到了f84d ed04 str.w lr, [sp, #-4]!，
再加上其他寄存器，我们可以做像e929 4803 stmdb r9!, {r0, r1, fp, lr}这样的事情，
它明确地使用non-lo寄存器作为地址和register-list中的stmdb助记符，
并反汇编为stmdb(FD=完全递减与DB=之前递减相同，用于stores.）
e、 我把这个放到foo.s中，这样我就可以在上面运行
arm-none-eabi-gcc -c(gcc -mthumb对于汇编来说似乎无关紧要，只是编译C）。
*/
///笔者注：这里可以直接使用pop和push指令

  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb

  
  
/*--------------------------------------------
 PendSV异常触发函数
---------------------------------------------*/
  .section	.text.RTS_PORT_TriggerPendsv
	.weak	RTS_PORT_TriggerPendsv
	.type	RTS_PORT_TriggerPendsv, %function


RTS_PORT_TriggerPendsv:

  ldr   r1, =SCB_ICSR_REG
  ldr   r2, [r1,#0]
  ldr   r3, =0x10000000
  orr   r2, r2, r3
  str   r2, [r1,#0]
  bx    lr



/*--------------------------------------------
 PendSV异常中断服务函数
---------------------------------------------*/
  .section	.text.PendSV_Handler
	.weak	PendSV_Handler
	.type	PendSV_Handler, %function


PendSV_Handler:
  /* 清除PendSV挂起标志位 */
  ldr   r1, =SCB_ICSR_REG
  ldr   r2, [r1,#0]
  orr   r2, r2, #0x80000000
  str   r2, [r1,#0]

  /* 获取rts_gb_curr_task_tcb->sp地址 */
  ldr   r3, =rts_gb_curr_task_tcb
  ldr   r3, [r3,#0]

  /* 判断是否为第一次调度，若是则跳转到set_once_entry */
  ldr   r1, =once_entry
  ldr   r2, [r1,#0]
  cbz   r2, set_once_entry

switch_top:
  /* 保存现场  */
  push  {r4-r11} 
  ldr   r1, =rts_gb_prev_task_tcb
  ldr   r1, [r1,#0]
  mrs   r2, msp
  str   r2, [r1,#0]
  
  /* rts_gb_prev_task_tcb = rts_gb_curr_task_tcb   */
  ldr   r1, =rts_gb_prev_task_tcb
  str   r3, [r1,#0]

switch_bottom: 
  /* light LED1 */
  /*
  ldr   r2, =GPIOD_ODR_REG
  ldr   r1, =0
  str   r1, [r2,#0]
  */
  
  /* 恢复现场  */
  ldr   r3, [r3,#0]
  msr   msp, r3
  pop   {r4-r11}
  bx    lr

set_once_entry:
  /* 设置  */
  movs r2, #1
  str  r2, [r1,#0]
  b    switch_bottom 






