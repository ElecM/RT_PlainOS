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
