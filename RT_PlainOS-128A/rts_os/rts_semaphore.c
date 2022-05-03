/**================================================================================================================
|
| 文件：rts_semaphore.c
| 说明：本文件是信号量源文件
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
#include "rts_semaphore.h"

#if (RTS_CFG_SEMAPHORE_ENB > 0u)

/*< 将TCB从尾部插入到信号量链表中   */
static void InsertTcbToSemaphoreList(TCB_t *tcb, semaphore_t *semaph)
{
   tcb->prev  = semaph->tcb.prev;
   tcb->next  = &semaph->tcb;
   semaph->tcb.prev->next = tcb;
   semaph->tcb.prev = tcb;
   semaph->tcb_num ++;
}

/*< 将TCB从信号量链表头部删除   */
static void RemoveTcbFromSmeaphoreList(TCB_t **tcb,semaphore_t* semaph)
{
   if(semaph->tcb_num > 0u)
   {
     (*tcb) = semaph->tcb.next;
     semaph->tcb.next->next->prev = &semaph->tcb;
     semaph->tcb.next = semaph->tcb.next->next;
     semaph->tcb_num -- ;
   }
}

/*< 动态创建信号量   */
semaphore_t* RTS_CreateSemaphore(s16_t initVal)
{
   semaphore_t *semaph = NULL;
   RTS_ENTER_CRITICAL
   semaph = (semaphore_t*)MemAlloc(sizeof(semaphore_t));  /*< 申请信号量所需的内存  */
   if(semaph != NULL)
   {
     semaph->tcb.next = &semaph->tcb;
     semaph->tcb.prev = &semaph->tcb;
     semaph->tcb_num   = 0u;
     semaph->value = initVal;
   }
   RTS_EXIT_CRITICAL
   return semaph;
}

/*< 动态销毁信号量   */
void RTS_DestorySemaphore(semaphore_t *semaph)
{
  RTS_ENTER_CRITICAL
   if(semaph != NULL)
   {
     MemFree(semaph);
   }
  RTS_EXIT_CRITICAL
}

/*< 初始化信号量   */
void RTS_InitSemaphore(semaphore_t* semaph, s16_t initVal)
{
  RTS_ENTER_CRITICAL
  semaph->value = initVal;
  semaph->tcb.next  = &semaph->tcb;
  semaph->tcb.prev  = &semaph->tcb;
  semaph->tcb_num   = 0u;
  RTS_EXIT_CRITICAL
}


/*< 等待信号量   */
void RTS_WaitSemaphore(semaphore_t* semaph)
{
  /*< 如果当前信号量小于0则将该任务移到该信号量链表中 */
  RTS_ENTER_CRITICAL
  rts_gb_curr_task_tcb->curr_status = RTS_TASK_STATUS_SYNC;
  rts_gb_curr_task_tcb->prev_status = RTS_TASK_STATUS_READY;
  --semaph->value;
  if(semaph->value < 0)
  {
    RTS_CORE_RemoveTcbFromRdyList((TCB_t *)rts_gb_curr_task_tcb);
    InsertTcbToSemaphoreList((TCB_t *)rts_gb_curr_task_tcb,semaph);
    RTS_CORE_UpdateTaskHightPrio();
    RTS_CORE_SwitchToHiprioTask();
    RTS_PORT_SwitchToNextTask();
  }
  RTS_EXIT_CRITICAL
}


/*< 释放信号量   */
void RTS_PostSemaphore(semaphore_t* semaph)
{
  TCB_t *tcb=NULL;
  RTS_ENTER_CRITICAL
  rts_gb_curr_task_tcb->curr_status = RTS_TASK_STATUS_READY;
  rts_gb_curr_task_tcb->prev_status = RTS_TASK_STATUS_SYNC;
  ++semaph->value;
  if(semaph->value <= 0)
  {
    RemoveTcbFromSmeaphoreList(&tcb,semaph);
    RTS_CORE_InsertTcbToRdyList(tcb);
    RTS_CORE_UpdateTaskHightPrio();
    RTS_CORE_SwitchToHiprioTask();
    RTS_PORT_SwitchToNextTask();
  }
  RTS_EXIT_CRITICAL
}


#endif
