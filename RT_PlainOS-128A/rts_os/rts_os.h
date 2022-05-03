/**================================================================================================================
|
| 文件：rts_os.c
| 说明：本文件RTS OS 内核头文件
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
| 2022.04.13 by SillyMem.
**==============================================================================================================**/
#ifndef __RTS_OS_H__
#define __RTS_OS_H__

#include "rts_cfg.h"
#include "rts_core.h"
#include "rts_task.h"
#include "rts_semaphore.h"
#include "datatype.h"


void RTS_OS_Init(void (*idle_task_hook)(void *));          /*<RTS OS初始化    */
void RTS_OS_Start(void);                                   /*<启动RTS OS  */
  


#endif
