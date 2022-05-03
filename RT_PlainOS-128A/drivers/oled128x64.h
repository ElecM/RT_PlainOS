#ifndef __OLED128X64_H__
#define __OLED128X64_H__

#include "rts_os/rts_cfg.h"
#include "rts_os/datatype.h"

typedef struct _oled128x64
{
   /** SPI底层接口 **/
   void (*init_pins)(void);
   void (*rst_module)(void);
   void (*write_data)(u8_t data);
   void (*write_cmd)(u8_t cmd);
   const u8_t (*chars6x8)[6];

   u8_t GRAM[8][128];
   /** oled API **/
   void (*fill_8bits_screen)(struct _oled128x64 *const mythis,u8_t screen);
   void (*disp_point)(struct _oled128x64 *const mythis,u8_t x,u8_t y);
   void (*erase_point)(struct _oled128x64 *const mythis,u8_t x,u8_t y);
   void (*refresh_screen)(struct _oled128x64 *const mythis);
   void (*disp_char6x8)(struct _oled128x64 *const mythis,u8_t x,u8_t y,s8_t ch);
   void (*disp_str6x8)(struct _oled128x64 *const mythis,u8_t x,u8_t y,const s8_t *str);
   /** hle：horizon little endian （水平小端）*/
   void (*disp_bin_pic_hle)(struct _oled128x64 *const mythis,u8_t x,u8_t y,u8_t w,u8_t h,const u8_t *pic);
   /** hle：horizon big endian （水平大端） */
   void (*disp_bin_pic_hbe)(struct _oled128x64 *const mythis,u8_t x,u8_t y,u8_t w,u8_t h,const u8_t *pic);
}oled128x64;



void oled128x64_init(oled128x64 *oled);



#endif // __OLED128X64_H__
