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
 * @brief ��ʾģ���ʼ��
 * 
 * @note ʹ�� LVGL �� st7789 LCD��Ļ
 */
void App_display_Init(void);

/**
 * @brief lcd��ʾ����
 * 
 * @param text 
 */
void App_display_ShowText(char *text);

/**
 * @brief lcd��ʾemoji����
 * 
 * @param emoji 
 */
void App_display_ShowEmoji(char * emoji_str);

#endif /* __APP_DISPLAY_H__ */
