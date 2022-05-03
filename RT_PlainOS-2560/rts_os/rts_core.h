/**===================================================================================
|
| 文件名：rts_core.h
| 文件说明：该文件用于实现RTS OS的内核部分的头文件。
| 概述说明: RTS OS支持时间片轮转调度和优先级抢占调度两种调度方式。
| 作者：ElecM  (1508555728@qq.com)
| 开源协议：MIT
| 版本：v2.02
|
| 2021.12.16 by ElecM
|
**==================================================================================**/
#ifndef __RTS_CORE_H__
#define __RTS_CORE_H__

#include "rts_cfg.h"
#include "rts_port.h"
#include "rts_heap1.h"
#include "datatype.h"


/*< 处理器位宽 */
#if   (RTS_CFG_CPU_DATA_BITS == 8u)
typedef u8_t taskStack_t;
#elif (RTS_CFG_CPU_DATA_BITS == 16u)
typedef u16_t taskStack_t;
#elif (RTS_CFG_CPU_DATA_BITS == 32u)
typedef u32_t taskStack_t;
#else
typedef u64_t taskStack_t;
#endif

//0:运行; 1:就绪态; 2:延时; 3:阻塞; 4:挂起; 5:中断服务态
#define RTS_TASK_STATUS_RUNNING  0  //运行态
#define RTS_TASK_STATUS_READY    1  //就绪态
#define RTS_TASK_STATUS_DELAY    2  //延时态
#define RTS_TASK_STATUS_BLOCK    3  //阻塞态
#define RTS_TASK_STATUS_PEND     4  //挂起态
#define RTS_TASK_STATUS_INTERR   5  //中断态
#define RTS_TASK_STATUS_REMOVED  6  //移除态
#define RTS_TASK_STATUS_INVALID  7  //无效状态
#define RTS_TASK_STATUS_SYNC     8  //任务同步状态


struct _taskListHead_t;

typedef struct _TCB_t                   /*< 声明任务控制块TCB */
{
    taskStack_t *sp;                    /*< 任务栈指针sp   */
    u8_t prio;                          /*< 任务优先级prio */
    u8_t curr_status;                   /*< 任务当前状态   */
    u8_t prev_status;                   /*< 任务前次状态   */
    u8_t tid;                           /*< 任务ID */
    
    #if  (RTS_CFG_DELAY_ENB > 0u)
    u32_t dly_coun;                     /*< 任务延时时间   */
    #endif

    struct _taskListHead_t *list_head;  /*< 任务链表头指针 */
    struct _TCB_t *next;                /*< 后一节点指针   */
    struct _TCB_t *prev;                /*< 前一节点指针   */

}TCB_t;


typedef struct _taskListHead_t          /*< 声明链表头     */
{
    TCB_t   *head;                      /*< 链表第一个节点指针 */
    u8_t  node_num;                     /*< 链表节点数量       */

}taskListHead_t;



extern volatile TCB_t * volatile rts_gb_curr_task_tcb; /*< 当前任务TCB指针 */
extern volatile u32_t  rts_gb_systicks;   /*< 任务系统的滴答计数器 */


/**======================================    RTS CORE OS API ===================================**/
void RTS_CORE_Init(void);                              /*< 内核初始化函数    */
u8_t RTS_CORE_UpdateTaskHightPrio(void);               /*< 更新就绪任务最高优先级    */
u8_t RTS_CORE_RegisterTask(TCB_t *tcb);                /*< 注册任务控制块    */
void RTS_CORE_InsertTcbToRdyList(TCB_t *tcb);          /*< 将任务控制块插入到就绪链表中    */
void RTS_CORE_RemoveTcbFromRdyList(TCB_t *tcb);        /*< 将任务控制块从就绪链表中移除    */
void RTS_CORE_InsertTcbToPendList(TCB_t *tcb);         /*< 将任务控制块插入到挂起链表中    */
void RTS_CORE_RemoveTcbFromPendList(TCB_t *tcb);       /*< 将任务控制块从挂起链表中移除    */
void RTS_CORE_InsertTcbToDlyList(TCB_t *tcb);          /*< 将任务控制块插入到延时链表中    */
void RTS_CORE_RemoveTcbFromDlyList(TCB_t *tcb);        /*< 将任务控制块从延时链表中移除    */
void RTS_CORE_DlyListHeadRelocate(TCB_t *tcb);         /*< 延时链表头重定位    */
void RTS_CORE_GetTcbFromTid(u8_t tid,TCB_t **tcb);     /*< 从Tid中获取TCB    */
void RTS_CORE_SetDlyListHead(TCB_t *tcb);              /*< 设置延时链表头   */
void RTS_CORE_GetDlyListHead(TCB_t **tcb);             /*< 获取延时链表头     */
void RTS_CORE_GetHightPrioTcb(TCB_t **tcb);            /*< 获取就绪任务的最高优先级TCB    */
u8_t RTS_CORE_GetHiprioTcbNum(void);                   /*< 获取最高优先级TCB个数 */

__attribute__ ((naked)) 
void RTS_CORE_SwitchToHiprioTask(void);                /*< 切换到最高优先级任务    */

__attribute__ ((naked)) 
void RTS_CORE_SwitchToNextTask(void);                  /*< 切换到下就绪链表中的下一个任务    */

void RTS_CORE_OsSchdule(void);                         /*< 进行一次任务调度    */
/**=========================================================================================**/

#endif // __RTS_CORE_H__
