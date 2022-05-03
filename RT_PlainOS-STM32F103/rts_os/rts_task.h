#ifndef __RTS_TASK_H__
#define __RTS_TASK_H__

#include "rts_core.h"
#include "rts_cfg.h"



u8_t RTS_TASK_IdleTaskInit(void (*hook)(void *));
//tid = 0的任务只能有内核使用,用户使用的tid \in [1,255]
//创建任务,返回tid, normal：1 <= tid <= RTS_CFG_MAX_TASKS_NUM
u8_t RTS_CreateTask(void (*task)(void *),                     /*< 创建任务函数  */
                    taskStack_t *stack,
                    u16_t       stack_size,
                    u8_t        prio);


void  RTS_PendTask(u8_t tid);                                  /*< 挂起任务  */
void  RTS_ResumeTask(u8_t tid);                                /*< 恢复任务  */
u8_t  RTS_GetCurrTaskTid(void);                                /*< 获取当前运行任务的tid    */
u8_t  RTS_GetCurrTaskPrio(void);                               /*< 获取当前运行任务的优先级 */
void  RTS_DelayTicks(u32_t ticks);                             /*< 系统滴答延时任务 */
void  RTS_ChangeTaskPrio(u8_t tid, u8_t dst_prio);             /*< 更改任务优先级 */
void  RTS_TurnOffSchdule(void);                                /*< 关闭任务调度器   */
void  RTS_TurnOnSchdule(void);                                 /*< 开启任务调度器   */
f32_t RTS_GetCpuUtilizationRate(void);                    /*< 获取CPU利用率   */




#endif
