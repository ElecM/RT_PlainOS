#include "gpio.h"


static void _set_data(struct _gpio *mythis,u8_t data)
{
    (*mythis->px_cofg.reg_out_data) = data;
}

static void _and_self_data(struct _gpio *mythis,u8_t data)
{
    (*mythis->px_cofg.reg_out_data) &= data;
}

static void _nor_self_data(struct _gpio *mythis,u8_t data)
{
    (*mythis->px_cofg.reg_out_data) |= data;
}

static u8_t _get_data(struct _gpio *mythis)
{
    return (*mythis->px_cofg.reg_in_data);
}


void gpio_init(gpio *gpiox)
{
    //获取寄存器的地址
    switch(gpiox->px_cofg.px)
    {
        case 'A':
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x39;
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x3A;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x3B;
            break;

        case 'B':
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x36;
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x37;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x38;
            break;

        case 'C':
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x33;
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x34;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x35;
            break;

        case 'D':
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x30;
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x31;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x32;
            break;

        case 'E':
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x21;
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x22;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x23;
            break;

        case 'F':
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x20;
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x61;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x62;
            break;

        default :
            gpiox->px_cofg.reg_dir      = (volatile u8_t *)0x66; //该寄存器保留，赋值将不会有任何效果
            gpiox->px_cofg.reg_in_data  = (volatile u8_t *)0x66;
            gpiox->px_cofg.reg_out_data = (volatile u8_t *)0x66;
    }
    //设置IO口的方向同时设置端口为上拉模式
    (*gpiox->px_cofg.reg_dir) = 0xff;//gpiox->px_cofg.dir_mask;
    (*gpiox->px_cofg.reg_out_data) = 0xff;

    //注册驱动函数
    gpiox->get_data = _get_data;
    gpiox->set_data = _set_data;
    gpiox->and_self_data = _and_self_data;
    gpiox->nor_self_data = _nor_self_data;
}

