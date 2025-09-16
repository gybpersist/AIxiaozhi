#include "Int_lcd.h"

static const char *TAG = "Int_lcd";

esp_lcd_panel_io_handle_t io_handle;
esp_lcd_panel_handle_t panel_handle;

/**
 * @brief lcd��ʾ����ʼ��
 *
 */
void Int_lcd_Init(void)
{
    // ���� gpio
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT};
    gpio_config(&bk_gpio_config);

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_SCLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    // ����spi����
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // ��LCD���ӵ�SPI����
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    // ��װ st7789 lcd����
    ESP_LOGI(TAG, "Install st7789 panel driver");
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);

    // ���� LCD ������
    esp_lcd_panel_reset(panel_handle);
    // ��ʼ��LCD������
    esp_lcd_panel_init(panel_handle);
    // ��ת��ɫ����λ��ת��ɫ�����У�
    esp_lcd_panel_invert_color(panel_handle, false);
    // ���ض����Ͼ���LCD������:
    esp_lcd_panel_mirror(panel_handle, false, false);

    // �û������ڴ���Ļ�򱳹�֮ǰ����Ԥ�����ģʽˢ�µ���Ļ��
    esp_lcd_panel_disp_on_off(panel_handle, true);

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);
}
