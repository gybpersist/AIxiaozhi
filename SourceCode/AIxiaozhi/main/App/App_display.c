#include "App_display.h"

// 加载中文字体
LV_FONT_DECLARE(font_puhui_14_1)

lv_obj_t *text_label;
lv_obj_t *emoji_label;

typedef struct Emotion
{
    const char *icon;
    const char *text;
} Emotion;

static const Emotion emotions[] = {
    {"😶", "neutral"},
    {"🙂", "happy"},
    {"😆", "laughing"},
    {"😂", "funny"},
    {"😔", "sad"},
    {"😠", "angry"},
    {"😭", "crying"},
    {"😍", "loving"},
    {"😳", "embarrassed"},
    {"😯", "surprised"},
    {"😱", "shocked"},
    {"🤔", "thinking"},
    {"😉", "winking"},
    {"😎", "cool"},
    {"😌", "relaxed"},
    {"🤤", "delicious"},
    {"😘", "kissy"},
    {"😏", "confident"},
    {"😴", "sleepy"},
    {"😜", "silly"},
    {"🙄", "confused"},
};

/**
 * @brief 显示模块初始化
 *
 * @note 使用 VAGL 和 st7789 LCD屏幕
 */
void App_display_Init(void)
{
    // 初始化LVGL
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,       /* LVGL task priority */
        .task_stack = 4096,       /* LVGL task stack size */
        .task_affinity = -1,      /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500, /* Maximum sleep in LVGL task */
        .timer_period_ms = 5      /* LVGL timer tick period in ms */
    };
    lvgl_port_init(&lvgl_cfg);

    // 添加LCD显示屏
    MY_LOGE("Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * 10,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = true,
        },
        .flags = {
            .buff_dma = true,
            .swap_bytes = true,
        }};
    lvgl_port_add_disp(&disp_cfg);

    // 默认显示一些图像
    // 创建一块屏幕
    lv_obj_t *screen = lv_screen_active();

    /* 任务锁定 */
    lvgl_port_lock(0);

    /* Your LVGL objects code here .... */
    /* Label */
    text_label = lv_label_create(screen);
    lv_label_set_long_mode(text_label, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(text_label, 130);
    lv_label_set_text(text_label, "            小智AI聊天机器人");
    lv_obj_set_style_text_font(text_label, &font_puhui_14_1, 0);
    lv_obj_align(text_label, LV_ALIGN_CENTER, 0, 40);

    emoji_label = lv_label_create(screen);
    lv_label_set_long_mode(emoji_label, LV_LABEL_LONG_WRAP); /*Break the long lines*/
    lv_label_set_text(emoji_label, "😎");
    lv_obj_set_width(emoji_label, LCD_H_RES); /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(emoji_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(emoji_label, font_emoji_64_init(), 0);
    lv_obj_align(emoji_label, LV_ALIGN_CENTER, 0, -50);

    lvgl_port_unlock();
}

/**
 * @brief lcd显示文字
 *
 * @param text
 */
void App_display_ShowText(char *text)
{
    /* 任务锁定 */
    lvgl_port_lock(0);

    char text_buf[256] = {0}; // 临时缓冲区
    // 拼接“前4个空格 + 动态文本”，避免缓冲区溢出
    snprintf(text_buf, sizeof(text_buf), "        %s", text);
    // 显示拼接后的文本
    lv_label_set_text(text_label, text_buf);
    // lv_label_set_text(text_label, text);
    lvgl_port_unlock();
}

/**
 * @brief lcd显示emoji表情
 *
 * @param emoji
 */
void App_display_ShowEmoji(char *emoji_str)
{
    // 计算emotions[]里面有几个表情
    size_t emoji_size = sizeof(emotions) / sizeof(Emotion);
    // 循环遍历对比显示的哪个表情
    for (size_t i = 0; i < emoji_size; i++)
    {
        if (strcmp(emotions[i].text, emoji_str) == 0)
        {
            /* 任务锁定 */
            lvgl_port_lock(0);
            lv_label_set_text(emoji_label, emotions[i].icon);
            lvgl_port_unlock();
            return;
        }
    }
}
