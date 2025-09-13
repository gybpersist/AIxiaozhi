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
 * @brief es8311 ���������ʼ��
 * 
 */
void Int_ES8311_Init(void);

/**
 * @brief ����˷��ȡ����
 * 
 * @param data ��ȡ��������
 * @param len Ҫ��ȡ�����ݳ���
 */
void Int_ES8311_ReadFromMic(void *data,int len);

/**
 * @brief д�뵽����
 * 
 * @param data д�������
 */
void Int_ES8311_WriteToSpeak(void *data,int len);

/**
 * @brief ����������С
 * 
 * @param Volume Ҫ���õ�����
 */
void Int_ES8311_SetVolume(int Volume);

/**
 * @brief ���þ���
 * 
 * @param Mute true:����  false:ȡ������
 */
void Int_ES8311_SetMute(bool Mute);

#endif /* __INT_ES8311_H__ */



