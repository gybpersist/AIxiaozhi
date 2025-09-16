#include "Int_WS2812.h"

// LED Strip object handle
led_strip_handle_t led_strip;

/**
 * @brief 初始化WS2812
 *
 */
void Int_WS2812_Init(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN,                        // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_COUNT,                             // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,                               // LED strip model
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB
        .flags = {
            .invert_out = false, // don't invert the output signal
        }};

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                    // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ,             // RMT counter clock frequency
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS, // the memory block size used by the RMT channel
        .flags = {
            .with_dma = LED_STRIP_USE_DMA, // Using DMA can improve performance when driving more LEDs
        }};

    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
}

/**
 * @brief 打开WS2812
 *
 * @param index 索引
 * @param red 红色值
 * @param green 绿色值
 * @param blue 蓝色值
 */
void Int_WS2812_LEDOpen(uint32_t index, uint32_t red, uint32_t green, uint32_t blue)
{
    led_strip_set_pixel(led_strip, index, red, green, blue);
    led_strip_refresh(led_strip);
}

/**
 * @brief 关闭WS2812
 *
 */
void Int_WS2812_LEDClose(void)
{
    led_strip_clear(led_strip);
    led_strip_refresh(led_strip);
}

