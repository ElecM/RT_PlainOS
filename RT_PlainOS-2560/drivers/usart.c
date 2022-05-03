#include "usart.h"
#include <avr/interrupt.h>


//定义全局this指针,主要用于中断服务函数
static volatile struct _usart *volatile global_this = NULL;


//12:osc  10:ubrr
static const u16_t ubrr_tbl[12][10]={
/**-----------------------------*/
{
    25,12,6,3,2,1,1,0,0,0
},
{
    47,23,11,7,5,3,2,1,1,0
},
{
    51,25,12,8,6,3,2,1,1,0
},
/**-----------------------------*/
{
    95,47,23,15,11,7,5,3,2,1
},
{
    103,51,25,16,12,8,6,3,2,1
},
{
    191,95,47,31,23,15,11,7,5,3,
},
/**-----------------------------*/
{
    207,103,51,34,25,16,12,8,6,3
},
{
    287,143,71,47,35,23,17,11,8,5
},
{
    383,191,95,63,47,31,23,15,11,7
},
/**------------------------------*/
{
    416,207,103,68,51,34,25,16,12,8
},
{
    479,239,119,79,59,39,29,19,14,9
},
{
    520,259,129,86,64,42,32,21,15,10
}
/**-------------------------------*/
};


static void _transmit_data(struct _usart *mythis,const u8_t data)
{

    while ( !( (*mythis->ux_cofg.reg_ucsra) & (1<<5)) )
    ;
    (*mythis->ux_cofg.reg_udr) = data;
}


static void _send_str(struct _usart *mythis,s8_t *str)
{
    while(*str)
    {
        mythis->transmit_data(mythis,*(str++));
    }
}


static u8_t _receive_data(struct _usart *mythis)
{
    while ( !((*mythis->ux_cofg.reg_ucsra) & (1<<7)) )
    ;
    return (*mythis->ux_cofg.reg_udr);
}




USART1_RX_INTERRUPT_FUN_NAME
{
    u8_t rev_tmp = *global_this->ux_cofg.reg_udr;
    if(rev_tmp == global_this->rev_ending_char)
    {
        //如果遇到结束符，则调用回调函数
        global_this->rev_data_callback_fun((struct _usart *)global_this);
    }
    else
    {
        *(global_this->rev_data_buff_ptr++) = rev_tmp;
    }
}


USART0_RX_INTERRUPT_FUN_NAME
{
    u8_t rev_tmp = *global_this->ux_cofg.reg_udr;
    if(rev_tmp == global_this->rev_ending_char)
    {
        //如果遇到结束符，则调用回调函数
        global_this->rev_data_callback_fun((struct _usart *)global_this);
    }
    else
    {
        *(global_this->rev_data_buff_ptr++) = rev_tmp;
    }
}




static void _disp_unsigned_integer(struct _usart *mythis,u32_t number)
{
    u32_t tmp = number/10;
    u8_t bits = 1;
    s8_t char_tmp[17]={0};
    while(tmp)
    {
        tmp /= 10;
        bits ++;
    }
    for(;bits>0;bits--)
    {
        char_tmp[bits-1] = number%10+'0';
        number /= 10;
    }
    mythis->send_str(mythis,char_tmp);
}


static void _disp_pic(struct _usart * const mythis,u8_t w,u8_t h,const u8_t* pic)
{
    u16_t i=0;
    u8_t  k;
    while(i<w*h)
    {
        for(k=0;k<8;k++)
        {
            if((*pic)&(1<<k))
            {
                mythis->transmit_data(mythis,'*');
                mythis->transmit_data(mythis,'*');
            }
            else
            {
                mythis->transmit_data(mythis,' ');
                mythis->transmit_data(mythis,' ');
            }


            i++;
            if((i %w) == 0)
            {
                mythis->transmit_data(mythis,'\r');
                mythis->transmit_data(mythis,'\n');
            }
        }
        pic++;
    }
}


void usart_init(usart * const usartn)
{
    global_this = usartn;
    switch(usartn->ux_cofg.number)
    {
        case 0:
          usartn->ux_cofg.reg_udr = (volatile u8_t *)0xc6;
          usartn->ux_cofg.reg_ucsra = (volatile u8_t *)0xc0;
          usartn->ux_cofg.reg_ucsrb = (volatile u8_t *)0xc1;
          usartn->ux_cofg.reg_ucsrc = (volatile u8_t *)0xc2;
          usartn->ux_cofg.reg_ubrrl = (volatile u8_t *)0xc4;
          usartn->ux_cofg.reg_ubrrh = (volatile u8_t *)0xc5;
          break;

        case 1:
          usartn->ux_cofg.reg_udr = (volatile u8_t *)0xCE;
          usartn->ux_cofg.reg_ucsra = (volatile u8_t *)0xC8;
          usartn->ux_cofg.reg_ucsrb = (volatile u8_t *)0xC9;
          usartn->ux_cofg.reg_ucsrc = (volatile u8_t *)0xCA;
          usartn->ux_cofg.reg_ubrrl = (volatile u8_t *)0xCC;
          usartn->ux_cofg.reg_ubrrh = (volatile u8_t *)0xCD;
          break;

        default:
          usartn->ux_cofg.reg_udr = (volatile u8_t *)0x97;   //保留，操作这些寄存器不会有任何效果
          usartn->ux_cofg.reg_ucsra = (volatile u8_t *)0x97;
          usartn->ux_cofg.reg_ucsrb = (volatile u8_t *)0x97;
          usartn->ux_cofg.reg_ucsrc = (volatile u8_t *)0x97;
          usartn->ux_cofg.reg_ubrrl = (volatile u8_t *)0x97;
          usartn->ux_cofg.reg_ubrrh = (volatile u8_t *)0x97;
          break;
    }

    //设置波特率和数据格式
    (*usartn->ux_cofg.reg_ubrrh) = 0;
    (*usartn->ux_cofg.reg_ubrrl) = 8;

    (*usartn->ux_cofg.reg_ucsrb) = ((1<<4)|(1<<3));
    (*usartn->ux_cofg.reg_ucsrc) = ((usartn->ux_cofg.crc_type<<4)|(usartn->ux_cofg.stop_bits_num<<3)|(usartn->ux_cofg.data_bits_num<<1));



    //注册驱动
    usartn->transmit_data = _transmit_data;
    usartn->receive_data  = _receive_data;
    usartn->send_str = _send_str;
    usartn->disp_pic = _disp_pic;
    usartn->disp_unsigned_integer = _disp_unsigned_integer;


}
