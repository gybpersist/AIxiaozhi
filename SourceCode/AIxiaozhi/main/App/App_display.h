#ifndef __APP_DISPLAY_H__
#define __APP_DISPLAY_H__

#include "esp_lvgl_port.h"
#include "font_emoji.h"
#include "font_puhui.h"
#include "Int_lcd.h"
#include "Com_Debug.h"

#define LCD_DRAW_BUFF_HEIGHT (50)
#define LCD_DRAW_BUFF_DOUBLE (1)

/**
 * @brief 显示模块初始化
 * 
 * @note 使用 LVGL 和 st7789 LCD屏幕
 */
void App_display_Init(void);

/**
 * @brief lcd显示文字
 * 
 * @param text 
 */
void App_display_ShowText(char *text);

/**
 * @brief lcd显示emoji表情
 * 
 * @param emoji 
 */
void App_display_ShowEmoji(char * emoji_str);

#endif /* __APP_DISPLAY_H__ */
