#include <avr/io.h>
#include "rts_os/rts_os.h"
#include "drivers/usart.h"
#include <util/atomic.h>


usart USART1={
    .ux_cofg.number = 0,
    .ux_cofg.osc_hz = OSC_16000000,
    .ux_cofg.ubrr   = UBRR_115200,
    .ux_cofg.data_bits_num = DATA_BITS_8,
    .ux_cofg.stop_bits_num = STOP_BITS_2,
    .ux_cofg.crc_type = CRC_FORBID,
    .ux_cofg.wait_or_interrupt = 0
};


//给空闲任务延时用，因为空闲任务中不能使用RTS_DelayTicks函数
void delay(uint16_t t)
{
    volatile unsigned int i;
    while(t--)
    {
        for(i=0;i<6000;i++)
            ;
    }
}


u8_t stack1[128];
u8_t stack2[128];
u8_t stack3[128];

void task1(void *data)
{
    data = data;
    u8_t rev;
    while (1)
    {
        USART1.send_str(&USART1,"  curr_task_tid:");
        USART1.disp_unsigned_integer(&USART1,RTS_GetCurrTaskTid());
        rev = USART1.receive_data(&USART1);
        if(rev == 'H')
        {
            RTS_PendTask(3);
        }
        else if(rev == 'A')
        {
            RTS_PendTask(1);
            
        }
        else if(rev == 'B')
        {
            RTS_ResumeTask(3);
        }
        else if(rev == 'C')
        {
            RTS_ChangeTaskPrio(3,1);
        }
        else if(rev == 'D')
        {
            RTS_ChangeTaskPrio(3,5);
        }
    } 
}


semaphore_t  *semaphore=NULL;


void task2(void *data)
{
    char ch;
    data = data;
    u32_t cr;
    while (1)
    {
        RTS_WaitSemaphore(semaphore);
        cr = RTS_GetCpuUtilizationRate()*1000.0001;
        USART1.send_str(&USART1,"ticks:");
        USART1.disp_unsigned_integer(&USART1,rts_gb_systicks);
        USART1.send_str(&USART1,"CPU Rate:");
        USART1.disp_unsigned_integer(&USART1,cr%100);
        USART1.send_str(&USART1,".");
        USART1.disp_unsigned_integer(&USART1,cr%10);
        USART1.send_str(&USART1,"%  ");
        RTS_PostSemaphore(semaphore);
        delay(100);
    
    } 
}

void task3(void *data)
{
    data = data;
    char ch;
    while (1)
    {
       RTS_WaitSemaphore(semaphore);
       USART1.send_str(&USART1,"task3 is running!  ");
       RTS_PostSemaphore(semaphore);
       delay(100);
       //RTS_DelayTicks(10);
    } 
}


//空闲任务里面不能使用RTS_DelayTicks()系统延时函数
void idle_hook(void *data)
{
    USART1.send_str(&USART1,"IDLE is running!  ");
    delay(100);
}



int main(void)
{
    // Insert code
    s8_t rev;
    DDRB |= 0XFF;
    PORTB = 0X00;
    usart_init(&USART1);
    USART1.send_str(&USART1,"RTS Start\r\n");

    RTS_OS_Init(idle_hook);
    RTS_CreateTask(task1,stack1,128,1);
    RTS_CreateTask(task2,stack2,128,2);
    RTS_CreateTask(task3,stack3,128,3);
    semaphore = RTS_CreateSemaphore(1);
    while (1)
    {
        rev = USART1.receive_data(&USART1);
        if(rev == 'S')
        {
            USART1.send_str(&USART1,"RTS_OS Started!");
            RTS_OS_Start();
        }
        else
            USART1.send_str(&USART1,"please input 'S' to satrt RTS OS\r\n");
    }
}



