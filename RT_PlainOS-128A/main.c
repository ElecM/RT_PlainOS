
#include <avr/io.h>
#include "rts_os/rts_os.h"
#include "drivers/usart.h"
#include "drivers/oled128x64.h"


usart USART1={
    .ux_cofg.number = 1,
    .ux_cofg.osc_hz = OSC_16000000,
    .ux_cofg.ubrr   = UBRR_115200,
    .ux_cofg.data_bits_num = DATA_BITS_8,
    .ux_cofg.stop_bits_num = STOP_BITS_2,
    .ux_cofg.crc_type = CRC_FORBID,
    .ux_cofg.wait_or_interrupt = 0
};

oled128x64 OLED;

//给空闲任务延时用，因为空闲任务中不能使用RTS_DelayTicks函数
void delay(u16_t t)
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
        RTS_DelayTicks(10);
    } 
}

semaphore_t *semaphore;


void task2(void *data)
{
    data = data;
    u32_t cr;
    while (1)
    {
        RTS_WaitSemaphore(semaphore);
        USART1.send_str(&USART1,"task 2 is running ");
        OLED.disp_str6x8(&OLED,0,0,"task 2       ");
        OLED.refresh_screen(&OLED);
        RTS_PostSemaphore(semaphore);
        RTS_DelayTicks(2);
    } 
}

void task3(void *data)
{
    data = data;
    while (1)
    {
       RTS_WaitSemaphore(semaphore);
       USART1.send_str(&USART1,"task3 is running  ");
       OLED.disp_str6x8(&OLED,0,0,"task 3 is running");
       OLED.refresh_screen(&OLED);
       RTS_PostSemaphore(semaphore);
       RTS_DelayTicks(20);
    }
}

//空闲任务里面不能使用RTS_DelayTicks()系统延时函数
void idle_hook(void *data)
{
    USART1.send_str(&USART1,"IDLE\r\n");
    delay(100);
}


int main(void)
{
    // Insert code
    s8_t rev;
    usart_init(&USART1);
    oled128x64_init(&OLED);
    OLED.refresh_screen(&OLED);
  
    DDRC |= 0XFF;
    PORTC = 0XFF;
    DDRG |= 0XFF;
    PORTG |= (u8_t)(1<<4);

    RTS_OS_Init(idle_hook);
    RTS_CreateTask(task1,stack1,128,1);
    RTS_CreateTask(task2,stack2,128,3);
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



