/**===================================================================================
|
| 文件名：rts_cfg.h
| 文件说明：该文件用于对RTS_OS进行配置（裁剪）,注意内存管理与内核无关。
| 概述说明: RTS OS支持时间片轮转调度和优先级抢占调度两种调度方式。
| 作者：ElecM  (1508555728@qq.com)
| 开源协议：MIT
| 版本：v2.02
|
| 2021.11.16 by ElecM
|
**==================================================================================**/
#ifndef __RTS_CFG_H__
#define __RTS_CFG_H__


/**----------------------------- CPU硬件参数配置 ----------------------------------**/
#define  RTS_CFG_CPU_DATA_BITS          (8u)       /*< CPU寄存器的大小(Byte) */

#define  RTS_CFG_CPU_ADDR_BITS          (16u)      /*< 地址的大小(Byte) */

/**-----------------------------  任务管理配置 ------------------------------------**/
#define  RTS_CFG_MAX_MAX_PRIORITIES    (6u)       /*< 最大优先级(0 <= prio <= 255) */

#define  RTS_CFG_MAX_TASKS_NUM         (10u)       /*< 最大任务数(2 <= num <= 255)  */

#define  RTS_CFG_DELAY_ENB              (0u)       /*< 任务延时功能 */

#define  RTS_CFG_IDLE_HOOK_ENB          (1u)       /*< 空闲任务的钩子函数功能 */

#define  RTS_CFG_SYS_UTILI_ENB          (0u)       /*< 统计系统利用率(尚未实现,后续版本填坑) */ 

#define  RTS_CFG_USE_MM                 (1u)       /*< 使用哪种内存管理(如果使用堆0，请提供MEM相关函数) */
//0:不使用内存管理 1：使用堆1
#define  RTS_CFG_GET_CPU_RATE_COUNT     (200u)    /*< 多少个滴答间隔计算一次CPU利用率 */

/**-----------------------------  堆1管理配置 ------------------------------------**/

#define  MEM_POOL_SIZE                 (1024u)     /*< 设置内存池大小(Byte) */

#define  MEM_ALC_BYT_TYP               u16_t       /*< 设置内存分配大小的类型，即每次最大分配字节的位数 */
// p = mem_alloc(MEM_ALC_BYT_TYP byte_size);

#define  MEM_BLK_SIZE                   (8u)       /*< 内存管理最小细粒度(Byte) */
//该项需要根据存储器数据总线宽度进行调整

#define  MEM_MCB_NUM                    (64u)      /*< 设置内存控制块的块数 */
//  16  <=  MEM_MCB_MAX_NUM <= 65535

#define  MEM_DATA_BITS                  (8u)      /*< 指定内存数据总线宽度 */

#define  MEM_ADDR_BITS                  (16u)      /*< 指定内存地址总线宽度 */

#define  MEM_DEBUG                      (0u)       /*< 是否开启debug */
#define  MemDebug(str)             puts(str)       /*< 开启debug需要提供 mem_debug(const char *s) */

#define  MEM_RT_CLE_FRAG                (0u)       /*< 实时内存碎片清理  */
// >= 1 : 开启实时内存碎片清理     0 ：手动清理内存碎片

#define  BYTE_ALIGN_INSTRU         __attribute((aligned(4)))  /*< 内存字节对齐的编译器指令 */

#define  CONST                     const           /*< 编译器的关键字 */


/**------------------------------------- 任务锁配置  -------------------------------------**/
#define  RTS_CFG_MUTEX_ENB              (0u)       /*< 互斥锁功能使能   */

#define  RTS_CFG_SEMAPHORE_ENB          (1u)       /*< 信号量功能使能   */


/**-------------------------------- 消息邮箱配置(尚未实现) ------------------------------------**/
#define  RTS_CFG_MAILBOX_ENB            (0u)       /*< 消息邮箱功能使能 */





#endif 
