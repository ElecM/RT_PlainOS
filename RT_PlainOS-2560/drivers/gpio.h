#ifndef __GPIO_H__
#define __GPIO_H__
#include "rts_os/rts_cfg.h"

typedef struct _gpio_cofg
{
    s8_t px;         //'A': "GPIOA"...
    volatile u8_t *volatile reg_dir ;     //IO口方向寄存器
    volatile u8_t *volatile reg_out_data; //IO口输出数据寄存器
    volatile u8_t *volatile reg_in_data;  //IO口输入数据寄存器
    u8_t enb_mask;   //使能掩码，1：开启 0：关闭
    u8_t dir_mask;   //方向掩码，1：输出 0：输入
}gpio_cofg;


typedef struct _gpio
{
    gpio_cofg px_cofg;    //GPIOX初始化配置
    void (*set_data)(struct _gpio *mythis,u8_t data);
    u8_t  (*get_data)(struct _gpio *mythis);
    void (*and_self_data)(struct _gpio *mythis,u8_t data);
    void (*nor_self_data)(struct _gpio *mythis,u8_t data);
}gpio;




void gpio_init(gpio *gpiox);

#endif // __GPIO_H__
