/**===================================================================================
|
| 文件名：rts_core.c
| 文件说明：该文件用于实现RTS OS的内核部分,对于单核CPU，RTS OS将运行态视为就绪态。
| 概述说明: RTS OS支持时间片轮转调度和优先级抢占调度两种调度方式。
| 作者：ElecM  (1508555728@qq.com)
| 开源协议：MIT
| 版本：v2.02
|
| 2021.12.16 by ElecM
|
**==================================================================================**/
#include "rts_core.h"


//最高优先级查询表
//这个表对应了0~255中最低位1所在的位置，因为数值越小优先级越高
//这里采用了优先级二级索引
//这里参考了ucosii的优先级获取方法，欲知详情请参考ucosii
//==== !!!!!!! 优先级数越小表示优先级越高 ====!!!!!!!//
const u8_t rts_gb_prio_tbl[256]=
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x00 to 0x0F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x10 to 0x1F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x20 to 0x2F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x30 to 0x3F */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x40 to 0x4F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x50 to 0x5F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x60 to 0x6F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x70 to 0x7F */
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x80 to 0x8F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0x90 to 0x9F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0xA0 to 0xAF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0xB0 to 0xBF */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0xC0 to 0xCF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0xD0 to 0xDF */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,  /* 0xE0 to 0xEF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0   /* 0xF0 to 0xFF */
};


//状态说明
//0:运行; 1:就绪态; 2:延时; 3:阻塞; 4:挂起; 5:中断服务态
//+++++表示任务不安全函数，*****表示任务安全函数，一般全局函数是任务安全的
#define MAX_BITMAP_ARRAY_NUM  (RTS_CFG_MAX_MAX_PRIORITIES/8+1)

static TCB_t *rts_gb_tasks_reg_tbl[RTS_CFG_MAX_TASKS_NUM+1]; /*< 定义任务注册表  */
static u8_t   rts_gb_prio_bitmap[MAX_BITMAP_ARRAY_NUM];     /*< 定义优先级位图 */
static u8_t   rts_gb_prio_bitmap_lv1_index[RTS_CFG_MAX_MAX_PRIORITIES/64+1]; /*< 定义优先级一级索引*/
static u8_t   rts_gb_prio_bitmap_lv2_index; /*< 定义优先级二级索引*/

static taskListHead_t  rts_gb_rdy_lh_tbl[RTS_CFG_MAX_MAX_PRIORITIES+1]; /*< 定义就绪链表数组 */

volatile TCB_t * volatile rts_gb_curr_task_tcb; /*< 定义当前任务TCB指针 */
volatile TCB_t * volatile rts_gb_prev_task_tcb; /*< 定义上一任务TCB指针 */

volatile u32_t  rts_gb_systicks;   /*< 定义任务系统的滴答计数器 */
static volatile u8_t hiprio = RTS_CFG_MAX_MAX_PRIORITIES;            /* 定义最高优先级  */

static taskListHead_t  rts_gb_pend_lh;   /*< 定义任务挂起链表头 */
static TCB_t           rts_pend_tcb0;    /*< 定义任务挂起TCB0  */

#if(RTS_CFG_DELAY_ENB > 0u)
static taskListHead_t  rts_gb_dly_lh;    /*< 定义任务延时链表头 */
static TCB_t           rts_dly_tcb0;     /*< 定义任务延时TCB0  */
#endif


/*---------------------------------- RTS_OS初始化部分 -------------------------------------*/
//RTS_OS任务注册表初始化
static void RTS_CORE_TasksRegTblInit(void)
{
    u8_t i;
    for(i=0;i<RTS_CFG_MAX_TASKS_NUM+1;i++)
    {
        rts_gb_tasks_reg_tbl[i]= NULL;
    }
}


//RTS_OS优先级位图初始化
static void RTS_CORE_PrioBitMapInit(void)
{
    u8_t i;
    for(i=0;i<MAX_BITMAP_ARRAY_NUM;i++)
    {
        rts_gb_prio_bitmap[i] = 0;
    }
}


//RTS_OS就绪任务链表数组初始化
static void RTS_CORE_RdyListHeadTblInit(void)
{
    u8_t i;
    for(i=0;i<RTS_CFG_MAX_MAX_PRIORITIES+1;i++)
    {
        rts_gb_rdy_lh_tbl[i].head = NULL;
        rts_gb_rdy_lh_tbl[i].node_num = 0;
    }
}

//RTS_OS任务挂起、延时链表头初始化
static void RTS_CORE_Pend_DlyListHeadInit(void)
{
    rts_pend_tcb0.curr_status = RTS_TASK_STATUS_INVALID;
    rts_pend_tcb0.prev_status = RTS_TASK_STATUS_INVALID;
    rts_pend_tcb0.prev = &rts_pend_tcb0;
    rts_pend_tcb0.next = &rts_pend_tcb0;
    rts_pend_tcb0.list_head = &rts_gb_pend_lh;
    rts_gb_pend_lh.head = &rts_pend_tcb0;
    rts_gb_pend_lh.node_num = 0;

    #if(RTS_CFG_DELAY_ENB > 0u)
    rts_pend_tcb0.dly_coun = 0;
    rts_dly_tcb0.curr_status = RTS_TASK_STATUS_INVALID;
    rts_dly_tcb0.prev_status = RTS_TASK_STATUS_INVALID;
    rts_dly_tcb0.prev = &rts_dly_tcb0;
    rts_dly_tcb0.next = &rts_dly_tcb0;
    rts_dly_tcb0.dly_coun = (u32_t)0xffffffffU;
    rts_dly_tcb0.list_head = &rts_gb_dly_lh;
    rts_gb_dly_lh.head = &rts_dly_tcb0;
    rts_gb_dly_lh.node_num = 0;
    #endif
}


void RTS_CORE_Init(void)
{
    RTS_CORE_TasksRegTblInit();
    RTS_CORE_PrioBitMapInit();
    RTS_CORE_RdyListHeadTblInit();
    RTS_CORE_Pend_DlyListHeadInit();
}

/*-----------------------------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++ 内核函数 +++++++++++++++++++++++++++++++++++++++++++*/
//注意：内核函数是不安全的函数，使用时必须进行临界区保护
//++++++获取就绪任务的最高优先级，通过查询方式获取最高优先级
u8_t RTS_CORE_UpdateTaskHightPrio(void)
{
    u8_t prio;
    u8_t bitmap_lv1_index;
    u8_t bitmap_index;
    u8_t bitmap_bit;
    
    bitmap_lv1_index = rts_gb_prio_tbl[rts_gb_prio_bitmap_lv2_index];
    bitmap_index     = rts_gb_prio_tbl[rts_gb_prio_bitmap_lv1_index[bitmap_lv1_index]];
    bitmap_bit       = rts_gb_prio_tbl[rts_gb_prio_bitmap[bitmap_lv1_index*8 + bitmap_index]];
    prio = bitmap_lv1_index*64 + bitmap_index*8 + bitmap_bit;
    hiprio = prio;
    return prio;
}

//根据优先级清除优先级位图
void RTS_CORE_ClearPrioBitmapBit(u8_t prio)
{
    rts_gb_prio_bitmap[prio/8] &= (u8_t)(~(1<<(prio%8)));
    if(rts_gb_prio_bitmap[prio/8] == 0)
    {
        rts_gb_prio_bitmap_lv1_index[prio/64] &= (u8_t)(~(1<<((prio%64)/8)));
        if(rts_gb_prio_bitmap_lv1_index[prio/64] == 0)
        {
            rts_gb_prio_bitmap_lv2_index  &= (u8_t)(~(1<<(prio/64)));
        }
    }
}

//根据优先级设置优先级位图
void RTS_CORE_SetPrioBitmapBit(u8_t prio)
{
   rts_gb_prio_bitmap[prio/8] |= (u8_t)(1<<(prio%8)); //更新优先级位图
   rts_gb_prio_bitmap_lv1_index[prio/64] |= (u8_t)(1<<((prio%64)/8));
   rts_gb_prio_bitmap_lv2_index  |= (u8_t)(1<<(prio/64));
}


//++++++注册任务到任务注册表中，注册成功后返回任务tid
//++++++返回0表示注册失败，规定0号tid用户不能使用
u8_t RTS_CORE_RegisterTask(TCB_t *tcb)
{
    u8_t tid;
    for(tid = 0;tid < RTS_CFG_MAX_TASKS_NUM+1;tid++)
    {
        //如果该位置没有被注册，则将TCB注册到该位置上
        if(rts_gb_tasks_reg_tbl[tid] == NULL) 
        {
            rts_gb_tasks_reg_tbl[tid] = tcb;
            tcb->tid = tid;
            break;
        }
    }
    return tid;
}

//++++++将任务TCB插入到任务就绪链表中
void RTS_CORE_InsertTcbToRdyList(TCB_t *tcb)
{
    u8_t prio;
    TCB_t *head = NULL;
    prio = tcb->prio;  //获取任务的优先级
    head = rts_gb_rdy_lh_tbl[prio].head;  //取得即将要插入就绪链表数组中的位置
    if(head == NULL)   //当就绪链表下没有就绪TCB时
    {
        rts_gb_rdy_lh_tbl[prio].head = tcb;  
        tcb->next = tcb;
        tcb->prev = tcb;
    }
    else               //当就绪链表下存在就绪TCB时
    {
        tcb->next = head;
        tcb->prev = head->prev;
        head->prev->next = tcb;
        head->prev = tcb;
    }
    ++rts_gb_rdy_lh_tbl[prio].node_num;  
    tcb->list_head = &rts_gb_rdy_lh_tbl[prio];
    //设置相应的优先级位图中的bit位
    RTS_CORE_SetPrioBitmapBit(prio);
}


//++++++将任务TCB从就绪链表中删除
void RTS_CORE_RemoveTcbFromRdyList(TCB_t *tcb)
{
    u8_t prio,num;
    prio = tcb->prio;
    num = rts_gb_rdy_lh_tbl[prio].node_num;
    if(num == 1)  //如果移除后就绪链表中的节点为0时
    {
        rts_gb_rdy_lh_tbl[prio].head = NULL;
        RTS_CORE_ClearPrioBitmapBit(prio);    //清除相应的优先级位图中的bit位
        rts_gb_rdy_lh_tbl[prio].node_num = 0;
    }
    else if(num > 1)
    {
        rts_gb_rdy_lh_tbl[prio].head = tcb->next;
        tcb->prev->next = tcb->next;
        tcb->next->prev = tcb->prev;
        --rts_gb_rdy_lh_tbl[prio].node_num;
    }
}


//++++++将任务TCB插入到挂起链表下
void RTS_CORE_InsertTcbToPendList(TCB_t *tcb)
{
    TCB_t *p;
    taskListHead_t *list = &rts_gb_pend_lh; 
    p = list->head;

    tcb->prev = p;        //总是将TCB插入到挂起链表中无效填充链表的下一个位置
    tcb->list_head = list;
    tcb->next = p->next; 
    p->next->prev = tcb;
    p->next = tcb;

    ++list->node_num;
}

//+++++将任务TCB从挂起链表下移除
void RTS_CORE_RemoveTcbFromPendList(TCB_t *tcb)
{
    tcb->prev->next = tcb->next;
    tcb->next->prev = tcb->prev;
    --rts_gb_pend_lh.node_num;
}


//+++++将任务TCB插入到延时链表中 (在后续版本中会加入错误处理)
#if(RTS_CFG_DELAY_ENB > 0u)
void RTS_CORE_InsertTcbToDlyList(TCB_t *tcb)
{
    //这里需要按照从小到大的顺序插入到延时链表中
    TCB_t *p;
    u32_t end_ticks = tcb->dly_coun + rts_gb_systicks; //计算到达的时间点
    taskListHead_t *list = &rts_gb_dly_lh;
    p = list->head;
    ++list->node_num;
    //此时滴答定时器溢出,此种情况的概率很小，大约1年才会发生一次
    if(end_ticks < rts_gb_systicks) 
    {
        //遍历到链表末尾，堆链表元素进行处理
        while(p->curr_status != RTS_TASK_STATUS_INVALID) 
        {
            p->dly_coun -= rts_gb_systicks;
            p = p->next;
        }
        end_ticks = tcb->dly_coun;
        rts_gb_systicks = 0;
    }
    p = list->head;  //开始重新插入
    //这里千万注意，如果后插进来的TCB块延时数等于当前数，则插入到该TCB后面
    while(end_ticks >= p->dly_coun)//找出要插入的位置
    {
       p = p->next;
    }
    tcb->next = p;  //开始将TCB插入到延时链表中
    tcb->prev = p->prev;
    p->prev->next = tcb;
    p->prev = tcb;
    //如果当前的计数最小，则让延时链表头指向该TCB
    if(end_ticks < list->head->dly_coun)
        list->head = tcb;
    //将延时时间到达的时刻写回TCB中
    tcb->dly_coun = end_ticks;
    tcb->list_head = &rts_gb_dly_lh;
}

//+++++将任务TCB从延时链表下移除
void RTS_CORE_RemoveTcbFromDlyList(TCB_t *tcb)
{ 
    //这里为了加快速度，不对延时链表头进行处理，延时链表头的处理需要在调用该函数时进行
    tcb->prev->next = tcb->next;
    tcb->next->prev = tcb->prev;
    --rts_gb_dly_lh.node_num;
}

//删除延时链表节点后，重新找到合适的延时链表头节点
void RTS_CORE_DlyListHeadRelocate(TCB_t *tcb)
{
    if(tcb == rts_gb_dly_lh.head)             //如果移除的是延时链表头，则将延时链表头指向下一个tcb
        rts_gb_dly_lh.head = tcb->next;
}


//指定延时链表头节点
void RTS_CORE_SetDlyListHead(TCB_t *tcb)
{
    rts_gb_dly_lh.head = tcb;
}

//获取延时链表表头节点
void RTS_CORE_GetDlyListHead(TCB_t **tcb)
{
    (*tcb) = rts_gb_dly_lh.head;
    
}


#endif

//根据tid获取TCB
void RTS_CORE_GetTcbFromTid(u8_t tid,TCB_t **tcb)
{
    *tcb = rts_gb_tasks_reg_tbl[tid];
}

void RTS_CORE_GetHightPrioTcb(TCB_t **tcb)
{
    *tcb = rts_gb_rdy_lh_tbl[hiprio].head;
}

//切换到最高优先级
void RTS_CORE_SwitchToHiprioTask(void)
{
    rts_gb_curr_task_tcb = rts_gb_rdy_lh_tbl[hiprio].head;
    /*< 如果不需要延时功能，则在没有同等优先级情况下关闭时间片轮转调度   */
    #if (RTS_CFG_DELAY_ENB == 0u)
    if(rts_gb_rdy_lh_tbl[hiprio].node_num > 1u)
        RTS_SYSTICK_ENABLE
    else
        RTS_SYSTICK_DISABLE

    #endif

}


//获取最高优先级TCB个数
u8_t RTS_CORE_GetHiprioTcbNum(void)
{
    return rts_gb_rdy_lh_tbl[hiprio].node_num;
}



//在优先级不改变情况下切换到下一个要运行的任务
void RTS_CORE_SwitchToNextTask(void)
{
    rts_gb_curr_task_tcb = rts_gb_prev_task_tcb->next;
}



//进行最高优先级调度
void RTS_CORE_OsSchdule(void)
{
    RTS_CORE_UpdateTaskHightPrio();
    RTS_CORE_SwitchToHiprioTask();
}



/*--------------------------------------------------------------------------------------------------*/

