/**================================================================================================================
|
| 文件：rts_semaphore.h
| 说明：本文件是信号量头文件
|       
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
| 2022.03.13 by SillyMem.
**==============================================================================================================**/
#ifndef __RTS_SEMAPHORE_H__
#define __RTS_SEMAPHORE_H__

#include "rts_cfg.h"
#include "rts_core.h"
#include "rts_port.h"
#include "rts_heap1.h"
#include "datatype.h"


#if (RTS_CFG_SEMAPHORE_ENB > 0u)


/*< 定义信号量类型 */
typedef struct _semaphore_t
{
   TCB_t tcb;                 /*< 定义TCB      */
   u16_t tcb_num;             /*< 定义TCB个数  */
   s16_t value;               /*< 定义信号量值 */
}semaphore_t; 


semaphore_t* RTS_CreateSemaphore(s16_t initVal);            /*< 动态创建信号量   */
void RTS_DestorySemaphore(semaphore_t *semaph);             /*< 动态销毁信号量   */
void RTS_InitSemaphore(semaphore_t* semaph, s16_t initVal); /*< 初始化信号量   */
void RTS_WaitSemaphore(semaphore_t* semaph);                /*< 等待信号量   */
void RTS_PostSemaphore(semaphore_t* semaph);                /*< 释放信号量   */





#endif


#endif
