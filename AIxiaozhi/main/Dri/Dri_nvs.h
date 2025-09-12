#ifndef __DRI_NVS_H__
#define __DRI_NVS_H__

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

/**
 * @brief ��ʼ�� nvs
 * 
 */
void Dri_nvs_Init(void);

/**
 * @brief ���Ҽ�
 * 
 * @param key ��
 * @return esp_err_t ������Ϣ 
 */
esp_err_t Dri_nvs_FindKey(char *key);

/**
 * @brief ���Ӽ�ֵ��
 * 
 * @param key ��
 * @param value ֵ
 */
void Dri_nvs_AddKey(char *key, char *value);

/**
 * @brief ��ȡ��ֵ��
 * 
 * @param key ��
 * @param out_value ��ȡ����ֵ
 * @param length ֵ����
 */
void Dri_nvs_GetKey(char *key, char *out_value, size_t *length);

#endif /* __DRI_NVS_H__ */
