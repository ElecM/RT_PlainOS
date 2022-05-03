#include "rts_task.h"
#include "USART.h"

#if(RTS_CFG_IDLE_HOOK_ENB > 0u)
static  void (* volatile rts_gb_idle_task_pf)(void *data); /*< 定义空闲任务钩子函数指针 */
#endif

static volatile u8_t       rts_gb_task_switch_on = 1;                 /*< 定义任务调度器开关 */
static volatile u16_t      rts_gb_task_idle_coun = 1;                 /*< 定义空闲任务计数器 */
static volatile u16_t      rts_gb_task_coun = 1;                      /*< 定义任务计数器 */ 
static volatile f32_t  rts_gb_cpu_utilization_rate = 0.000001;    /*< CPU利用率 */


/*----------------------------------------- 任务初始化 ---------------------------------------------------*/
//定义空闲任务
static void IdleTask(void *data)
{
    data = data;
    while(1)
    {
        #if(RTS_CFG_IDLE_HOOK_ENB > 0u)
        if(rts_gb_idle_task_pf != NULL)  //空闲任务的钩子函数指针
           rts_gb_idle_task_pf(data);
        #endif
    }
}

//空闲任务初始化
u8_t RTS_TASK_IdleTaskInit(void (*hook)(void *))
{
    static taskStack_t idle_stack[128];
    u8_t tid;
    tid = RTS_CreateTask(IdleTask,idle_stack,128,RTS_CFG_MAX_MAX_PRIORITIES);
    #if(RTS_CFG_IDLE_HOOK_ENB > 0u)
    rts_gb_idle_task_pf = hook;
    #endif
    return tid;
}

/*--------------------------------------------------------------------------------------------------------*/


/*============================================== RTS TASK API ========================================*/
//RTS OS API是任务安全函数

//******创建任务,成功返回tid，失败返回0
u8_t RTS_CreateTask(void (*task)(void *),
                    taskStack_t *stack,
                    u16_t       stack_size,
                    u8_t        prio)
{
    TCB_t *task_tcb = NULL;
    u8_t tid = 0;
    //提前初始化堆栈,尽可能减少临界区访问时间
    stack = RTS_PORT_TaskStackInit(task,stack,stack_size);
    RTS_ENTER_CRITICAL
    //一旦分配成功并创建成功，则由任务注册表来接管该TCB
    task_tcb = (TCB_t *)MemAlloc(sizeof(TCB_t)); 
    if(task_tcb)
    {   
        tid = RTS_CORE_RegisterTask(task_tcb);
        if(tid >= 0)
        { 
            task_tcb->tid  = tid;
            task_tcb->prio = prio;
            task_tcb->curr_status = RTS_TASK_STATUS_READY;
            task_tcb->prev_status = RTS_TASK_STATUS_REMOVED;
            task_tcb->sp = stack;
            //将任务TCB挂载到任务就绪链表中，同时更新优先级位图
            RTS_CORE_InsertTcbToRdyList(task_tcb);
        }
        else
        {
            MemFree(task_tcb);
        }
    }
    RTS_EXIT_CRITICAL
    return tid;
}


//根据tid挂起任务
void RTS_PendTask(u8_t tid)
{
    TCB_t * tcb;
    RTS_ENTER_CRITICAL
    RTS_CORE_GetTcbFromTid(tid,&tcb);
    if(tcb->curr_status != RTS_TASK_STATUS_SYNC)
    {
        //如果挂起的是自己,则获取最高优先级任务运行
        if(tcb == rts_gb_curr_task_tcb)
        {
            if(tcb->curr_status == RTS_TASK_STATUS_READY)
            {
                tcb->curr_status = RTS_TASK_STATUS_PEND;
                tcb->prev_status = RTS_TASK_STATUS_READY;
                RTS_CORE_RemoveTcbFromRdyList(tcb);
                RTS_CORE_InsertTcbToPendList(tcb);
                RTS_CORE_UpdateTaskHightPrio();
                RTS_CORE_SwitchToHiprioTask();
            }
        }
        else
        {
            if(tcb->curr_status == RTS_TASK_STATUS_READY) //从就绪态挂起
            {
                tcb->curr_status = RTS_TASK_STATUS_PEND;
                tcb->prev_status = RTS_TASK_STATUS_READY;
                RTS_CORE_RemoveTcbFromRdyList(tcb);      
                RTS_CORE_InsertTcbToPendList(tcb);
                RTS_CORE_OsSchdule();
            }

            #if(RTS_CFG_DELAY_ENB > 0u)
            else if(tcb->curr_status == RTS_TASK_STATUS_DELAY) //从延时态挂起
            {
                tcb->curr_status = RTS_TASK_STATUS_PEND;
                tcb->prev_status = RTS_TASK_STATUS_DELAY;
                RTS_CORE_RemoveTcbFromDlyList(tcb); //移除后，tcb的下一个指针未变，不影响tcb的下一个指向
                RTS_CORE_DlyListHeadRelocate(tcb);  //让延时链表头指向新的链表节点
                RTS_CORE_InsertTcbToPendList(tcb);
                RTS_CORE_OsSchdule();
            }
            #endif
        }
        RTS_TRIGGER_PENDSV
    }
    RTS_EXIT_CRITICAL
}


//根据tid恢复任务
void RTS_ResumeTask(u8_t tid)
{
    TCB_t * tcb;
    RTS_ENTER_CRITICAL
    RTS_CORE_GetTcbFromTid(tid,&tcb);
    if(tcb->curr_status == RTS_TASK_STATUS_PEND)
    {
        //从挂起态恢复到就绪态，这里不做超时处理，只是简单地处理为忽略中间挂起时间而直接进入就绪态
        tcb->curr_status = RTS_TASK_STATUS_READY;
        tcb->prev_status = RTS_TASK_STATUS_PEND;
        RTS_CORE_RemoveTcbFromPendList(tcb);
        RTS_CORE_InsertTcbToRdyList(tcb);
        RTS_CORE_OsSchdule();
        RTS_TRIGGER_PENDSV
    }
    RTS_EXIT_CRITICAL
}


//系统滴答延时函数
#if(RTS_CFG_DELAY_ENB > 0u)
void RTS_DelayTicks(u32_t ticks)
{
    if(ticks >= 2)
    {
        RTS_ENTER_CRITICAL
        rts_gb_curr_task_tcb->dly_coun = ticks;

        RTS_CORE_RemoveTcbFromRdyList((TCB_t *)rts_gb_curr_task_tcb);
        RTS_CORE_InsertTcbToDlyList((TCB_t *)rts_gb_curr_task_tcb);
        rts_gb_curr_task_tcb->prev_status = RTS_TASK_STATUS_READY; //这里不进行运行态处理，因为只考虑单核处理器
        rts_gb_curr_task_tcb->curr_status = RTS_TASK_STATUS_DELAY;
        RTS_CORE_UpdateTaskHightPrio();
        RTS_CORE_SwitchToHiprioTask();
        RTS_EXIT_CRITICAL
        
        RTS_TRIGGER_PENDSV
    }
}
#endif


//获取当前运行任务的tid
u8_t RTS_GetCurrTaskTid(void)
{
    return rts_gb_curr_task_tcb->tid;
}

//获取当前运行任务的优先级
u8_t RTS_GetCurrTaskPrio(void)
{
    return rts_gb_curr_task_tcb->prio;
}



//改变任务优先级
void RTS_ChangeTaskPrio(u8_t tid, u8_t dst_prio)
{
    TCB_t *tcb;
    RTS_ENTER_CRITICAL
    RTS_CORE_GetTcbFromTid(tid,&tcb);
    if(tcb->prio != dst_prio)
    {
        if(tcb->curr_status != RTS_TASK_STATUS_SYNC)
        {
            //如果当前的TCB是处于就绪状态，则需要将优先级更改后的TCB插入到新的位置
            if(tcb->curr_status == RTS_TASK_STATUS_READY)
            {
                RTS_CORE_RemoveTcbFromRdyList(tcb);
                tcb->prio = dst_prio;
                RTS_CORE_InsertTcbToRdyList(tcb);
                RTS_CORE_UpdateTaskHightPrio();
                RTS_CORE_SwitchToHiprioTask();
                RTS_TRIGGER_PENDSV
            }
            else
            {
                tcb->prio = dst_prio;
            }
        }
    }
    RTS_EXIT_CRITICAL
}


/*< RTS系统滴答定时器的中断服务函数 */
RTS_SYSTICK_HANDLER
{
   
   //rts_gb_curr_task_tcb = rts_gb_prev_task_tcb->next;
   //RTS_TRIGGER_PENDSV
   
   
   #if(RTS_CFG_DELAY_ENB > 0u)
   TCB_t * dly_tcb;
   TCB_t * dly_tcb_next;
   #endif
   GPIOD->ODR = ~GPIOD->ODR;
   ++rts_gb_systicks;
   
   RTS_SYSTICK_HANDLER_ENTER_DO
   if(rts_gb_task_switch_on == 1)
   {
        RTS_CORE_SwitchToNextTask();
        #if(RTS_CFG_DELAY_ENB > 0u)
        RTS_CORE_GetDlyListHead(&dly_tcb);
        

        if(rts_gb_systicks == dly_tcb->dly_coun)  //如果延时时间到了，就恢复任务
        {
            //GPIOD->ODR = ~GPIOD->ODR;
            //恢复所有延时时间到达的任务
            while(dly_tcb->dly_coun == rts_gb_systicks)  
            {
                dly_tcb->dly_coun = 0;
                dly_tcb_next = dly_tcb->next;
                RTS_CORE_RemoveTcbFromDlyList(dly_tcb);
                dly_tcb->curr_status = RTS_TASK_STATUS_READY;
                dly_tcb->prev_status = RTS_TASK_STATUS_DELAY;
                RTS_CORE_InsertTcbToRdyList(dly_tcb);
                dly_tcb = dly_tcb_next;
            }
            //转移延时链表上的控制块后，将延时链表指针指向下一个
            RTS_CORE_SetDlyListHead(dly_tcb);
            RTS_CORE_UpdateTaskHightPrio();
            RTS_CORE_SwitchToHiprioTask();
        }
        #endif
        RTS_TRIGGER_PENDSV
    }
    

    //统计CPU利用率
    if(rts_gb_curr_task_tcb->prio == RTS_CFG_MAX_MAX_PRIORITIES)
    {  
       rts_gb_task_idle_coun ++;
    }
    rts_gb_task_coun ++;
    if(rts_gb_task_coun >= RTS_CFG_GET_CPU_RATE_COUNT)
    {
        rts_gb_task_coun = 1;
        rts_gb_task_idle_coun = 1;
    }

    RTS_SYSTICK_HANDLER_EXIT_DO
    RTS_SYSTICK_CLEAN_INT_FLAG

 
    
    

}


/*< 获取CPU利用率   */
f32_t RTS_GetCpuUtilizationRate(void)
{
    return rts_gb_cpu_utilization_rate = 1.000001 - (f32_t)rts_gb_task_idle_coun/(f32_t)rts_gb_task_coun;
}


/*< 关闭任务调度器   */
void RTS_TurnOffSchdule(void)
{
    RTS_ENTER_CRITICAL
    rts_gb_task_switch_on = 0;
    RTS_EXIT_CRITICAL
}

/*< 开启任务调度器   */
void RTS_TurnOnSchdule(void)
{
    RTS_ENTER_CRITICAL
    rts_gb_task_switch_on = 1;
    RTS_EXIT_CRITICAL
}


/*================================================================================================*/

