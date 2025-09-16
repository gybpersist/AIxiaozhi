#ifndef __INT_WS2812_H__
#define __INT_WS2812_H__

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"

// Set to 1 to use DMA for driving the LED strip, 0 otherwise
// Please note the RMT DMA feature is only available on chips e.g. ESP32-S3/P4
#define LED_STRIP_USE_DMA 0

// Numbers of the LED in the strip
#define LED_STRIP_LED_COUNT 2
#define LED_STRIP_MEMORY_BLOCK_WORDS 0 // let the driver choose a proper memory block size automatically

// GPIO assignment
#define LED_STRIP_GPIO_PIN 46

// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)

/**
 * @brief 初始化WS2812
 * 
 */
void Int_WS2812_Init(void);

/**
 * @brief 打开WS2812
 * 
 * @param index 索引
 * @param red 红色值
 * @param green 绿色值
 * @param blue 蓝色值
 */
void Int_WS2812_LEDOpen(uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

/**
 * @brief 关闭WS2812
 * 
 */
void Int_WS2812_LEDClose(void);

#endif /* __INT_WS2812_H__ */
