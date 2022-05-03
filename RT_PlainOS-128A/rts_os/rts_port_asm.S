/*===========================================================================
|
|  文件：rts_port_asm.S（avr-gcc）
|  说明：该文件主要实现任务切换，将上一个任务控制块rts_gb_prev_task_tcb切换为
|       当前任务控制块rts_gb_curr_task_tcb
|
|  作者：lishanwu  (1508555728@qq.com | SillyMem | 0个错误0个警告)
|  版本：v2.7
|  时间：2022.04.11
|  附加：本文件完全可以使用内联汇编来完成，但是需要引入裸函数__attribute__((naked))，
|        为了不想写出不伦不类的代码(狗头保命)，这里还是采用纯汇编来编写
|
===========================================================================*/
  
  .extern rts_gb_prev_task_tcb        /* 引入上一任务控制块指针 */
  .extern rts_gb_curr_task_tcb        /* 引入当前任务控制块指针 */

  .global RTS_PORT_SwitchToNextTask   /* 导出上下文切换函数   */



/*------------------------------------------------------
  data段声明--定义数据段--定义一个全局变量作为初始任务标志
-------------------------------------------------------*/
  .section .data
  
once_entry:
  .byte 0x0

/*------------------------------------------------------
  text段声明--定义text段--双字节对齐
-------------------------------------------------------*/
  .section .text  
  .align 1

RTS_PORT_SwitchToNextTask:
  /* 任务切换上半部分  */
  push  r29       /*< R29入栈   */
  push  r28       /*< R28入栈   */
  push	r31
  push	r30
  push	r0        /*< R0入栈    */
  lds	  r0, 0x5f  /*< SREG入栈  */
  push	r0
  lds	  r0, 0x5B  /*< RAMPZ入栈 */
  push	r0
  push  r16       /*< R16入栈   */
  push	r17       /*< R17入栈   */
  push  r18       /*< ...入栈   */
  push	r19
  push  r20
  push	r21
  push  r22
  push	r23
  push	r24
  push  r25
  push	r26
  push	r27
  push  r1        /* 可以不push，r1===0  */
  push  r2
  push	r3
  push  r4
  push	r5
  push  r6
  push	r7
  push  r8
  push	r9
  push  r10
  push	r11
  push  r12
  push	r13
  push  r14
  push	r15
  
  /* 判断是否为初次任务，如果是，则跳转到set_rtsgbonceentry_to_1 */
  lds   r30, once_entry
  cpi   r30, 0
  breq  set_onceentry_to_1


  /*< 将SP保存到上一任务的SP  */
  lds   r30, rts_gb_prev_task_tcb
  lds   r31, rts_gb_prev_task_tcb+1
  in    r24, 0x3d
  in    r25, 0x3e
  std   Z+1, r25
  st    Z,   r24

  /* rts_gb_prev_task_tcb = rts_gb_curr_task_tcb  */
  lds   r26, rts_gb_curr_task_tcb
  lds   r27, rts_gb_curr_task_tcb+1
  sts   rts_gb_prev_task_tcb,  r26
  sts   rts_gb_prev_task_tcb+1,r27

set_onceentry_to_1:
  ldi   r30, 1
  sts   once_entry,  r30

switch_bottom:
  /* 任务切换下半部分  */
  /*< 将SP指向下一个要运行的任务SP   */
  lds  r30,rts_gb_curr_task_tcb
  lds  r31,rts_gb_curr_task_tcb+1
  ldd  r25, Z+1
  ld   r24, Z
  out  0x3d,r24
  out  0x3e,r25
  
  pop	  r15     /*< 各寄存器出栈，恢复任务环境   */
  pop   r14
  pop	  r13
  pop   r12
  pop	  r11
  pop   r10
  pop	  r9
  pop   r8
  pop	  r7
  pop   r6
  pop	  r5
  pop   r4
  pop	  r3
  pop   r2
  pop   r1
  pop	  r27
  pop	  r26
  pop   r25
  pop	  r24
  pop	  r23
  pop   r22
  pop	  r21
  pop   r20
  pop	  r19
  pop   r18
  pop	  r17
  pop   r16
  pop	  r0
  sts	  0x5B, r0   /*< RAMPZ出栈    */
  pop   r0         /*< SREG出栈     */
  sts   0x5f, r0
  pop	  r0
  pop	  r30
  pop	  r31
  pop	  r28
  pop	  r29
  ret



