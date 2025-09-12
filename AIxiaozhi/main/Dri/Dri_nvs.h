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
 * @brief 初始化 nvs
 * 
 */
void Dri_nvs_Init(void);

/**
 * @brief 查找键
 * 
 * @param key 键
 * @return esp_err_t 错误信息 
 */
esp_err_t Dri_nvs_FindKey(char *key);

/**
 * @brief 增加键值对
 * 
 * @param key 键
 * @param value 值
 */
void Dri_nvs_AddKey(char *key, char *value);

/**
 * @brief 获取键值对
 * 
 * @param key 键
 * @param out_value 获取键的值
 * @param length 值长度
 */
void Dri_nvs_GetKey(char *key, char *out_value, size_t *length);

#endif /* __DRI_NVS_H__ */
