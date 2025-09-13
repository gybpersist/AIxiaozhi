#ifndef __INT_ES8311_H__
#define __INT_ES8311_H__

#include "driver/i2s_std.h"
#include "driver/i2s_tdm.h"
#include "soc/soc_caps.h"

#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "unity.h"

#include "driver/i2c.h"
#include "driver/gpio.h"

#define ES8311_I2C_SDA_PIN GPIO_NUM_0 
#define ES8311_I2C_SCL_PIN GPIO_NUM_1

#define ES8311_I2S_BCK_PIN GPIO_NUM_2 
#define ES8311_I2S_MCK_PIN GPIO_NUM_3
#define ES8311_I2S_DI_PIN GPIO_NUM_4
#define ES8311_I2S_WS_PIN GPIO_NUM_5
#define ES8311_I2S_DO_PIN GPIO_NUM_6

#define ES8311_PA_EN_PIN GPIO_NUM_7

/**
 * @brief es8311 编解码器初始化
 * 
 */
void Int_ES8311_Init(void);

/**
 * @brief 从麦克风读取数据
 * 
 * @param data 读取到的数据
 * @param len 要读取的数据长度
 */
void Int_ES8311_ReadFromMic(void *data,int len);

/**
 * @brief 写入到功放
 * 
 * @param data 写入的数据
 */
void Int_ES8311_WriteToSpeak(void *data,int len);

/**
 * @brief 设置音量大小
 * 
 * @param Volume 要设置的音量
 */
void Int_ES8311_SetVolume(int Volume);

/**
 * @brief 设置静音
 * 
 * @param Mute true:静音  false:取消静音
 */
void Int_ES8311_SetMute(bool Mute);

#endif /* __INT_ES8311_H__ */



