/*
 * ************************************************
 * 
 *              STM32 blink gcc demo
 * 
 *  CPU: STM32F103C8
 *  PIN: PA1
 * 
 * ************************************************
*/
#include "stm32f10x.h"
#include "rts_os.h"
#include "USART.h"

void delay(int x)
{
    volatile int i,j;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < 1000; j++)
            __NOP();
    }
}

__attribute__((aligned(8))) u32_t  stack2[512];
__attribute__((aligned(8))) u32_t  stack3[512];


semaphore_t *semaphore;

void task2(void *data)
{
    data = data;
    while (1)
    {
        RTS_WaitSemaphore(semaphore);
        USART1_PrintString("TASK2 IS RUNNING   ");
        RTS_PostSemaphore(semaphore);
        RTS_DelayTicks(10);
    } 
}

void task3(void *data)
{
    data = data;
    while (1)
    {
        RTS_WaitSemaphore(semaphore);
        USART1_PrintString("task3  is running   ");
        RTS_PostSemaphore(semaphore);
        RTS_DelayTicks(10);
    }
}


void idel_task(void *data)
{
    USART1_PrintString("idle task is running   ");
    delay(1000);
}



int main()
{
    //开启GPIOB端口时钟
    RCC->APB2ENR |= (1<<5);
    //清空控制PB0的端口位
    GPIOD->CRL &= ~( 0x0F<< (4*2));
    //配置PB0为通用推挽输出，速度为10M
    GPIOD->CRL |= (1<<4*2);
    //PB0输出低电平
    GPIOD->ODR = 0xff;
    USART1_Init();
    USART1_PrintString("RTS Start\r\n");
    
    

    RTS_OS_Init(idel_task);
    if(RTS_CreateTask(task2,stack2,512,2)==0)
        USART1_PrintString("task2 create fail\r\n");
    if(RTS_CreateTask(task3,stack3,512,2) == 0)
        USART1_PrintString("task3 create fail\r\n");

    *((volatile u32_t *)0xE000ED22) = 0xff;
    semaphore = RTS_CreateSemaphore(1);
    RTS_OS_Start();
   

    while (1)
    {
        ;
    }
}




