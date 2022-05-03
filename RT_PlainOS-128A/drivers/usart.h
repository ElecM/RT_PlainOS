#ifndef __USART_H__
#define __USART_H__

#include "rts_os/datatype.h"


//串口的接收数据中断服务函数名
#define USART0_RX_INTERRUPT_FUN_NAME   ISR(USART0_RX_vect)
#define USART1_RX_INTERRUPT_FUN_NAME   ISR(USART1_RX_vect)


//可供选择的晶振频率
typedef enum _selectable_osc
{
    OSC_1000000 = 0U,
    OSC_1843200,
    OSC_2000000,
    OSC_3686400,
    OSC_4000000,
    OSC_7372800,
    OSC_8000000,
    OSC_11059200,
    OSC_14745600,
    OSC_16000000,
    OSC_18432000,
    OSC_20000000
}selectable_osc;


//可供选择的波特率
typedef enum _selectable_ubrr
{
    UBRR_2400 = 0U,
    UBRR_4800,
    UBRR_9600,
    UBRR_14400,
    UBRR_19200,
    UBRR_28800,
    UBRR_38400,
    UBRR_57600,
    UBRR_76800,
    UBRR_115200

}selectable_ubrr;

typedef enum _selectable_crc_type
{
    CRC_FORBID = 0U,
    CRC_RESERVE,
    CRC_EVEN,
    CRC_ODD

}selectable_crc_type;


typedef enum _selectable_stop_bits
{
    STOP_BITS_1 = 0U,
    STOP_BITS_2
}selectable_stop_bits;


typedef enum _selectable_data_bits
{
    DATA_BITS_5 = 0U,
    DATA_BITS_6,
    DATA_BITS_7,
    DATA_BITS_8

}selectable_data_bits;


typedef struct _usart_cofg
{
    u8_t number;//usart-number
    //usart相关寄存器
    selectable_osc osc_hz;
    selectable_ubrr ubrr;
    selectable_crc_type crc_type;
    selectable_data_bits data_bits_num;
    selectable_stop_bits stop_bits_num;
    u8_t wait_or_interrupt;
    volatile u8_t *volatile reg_udr;
    volatile u8_t *volatile reg_ucsra;
    volatile u8_t *volatile reg_ucsrb;
    volatile u8_t *volatile reg_ucsrc;
    volatile u8_t *volatile reg_ubrrl;
    volatile u8_t *volatile reg_ubrrh;
}usart_cofg;


typedef struct _usart
{
    usart_cofg ux_cofg;
    volatile u8_t rev_ending_char;
    volatile u8_t *volatile rev_data_buff_ptr;
    void (*transmit_data)(struct _usart *mythis,const u8_t data);
    void (*send_str)(struct _usart *mythis,s8_t *str);
    u8_t  (*receive_data)(struct _usart *mythis);
    void (*rev_data_callback_fun)(struct _usart *mythis);
    void (*disp_pic)(struct _usart * const mythis,u8_t w,u8_t h,const u8_t* pic);
    void (*disp_unsigned_integer)(struct _usart *mythis,u32_t number);

}usart;


void usart_init(usart * const usartn);

#endif // __USART_H__
