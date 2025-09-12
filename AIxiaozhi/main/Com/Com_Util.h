#ifndef __COM_UTIL_H__
#define __COM_UTIL_H__

#include "esp_heap_caps.h"
#include "string.h"
#include "stddef.h"
#include "assert.h"
#include "Com_Debug.h"
#include <esp_wifi.h>
#include "Dri_nvs.h"

#define UUID_KEY "UUID"

/**
 * 获取WIFI的 mac地址
 */
char *Com_Get_MAC(void);

/**
 * @brief 获取一个UUID 
 * 
 * @return char* 获取到的UUID
 */
char *Com_Get_UUID(void);

// 定义 可变缓冲区
typedef struct mutable_buffer mutable_buffer_t;

/**
 * @brief 创建 可变缓冲区对象
 * 
 * @return mutable_buffer_t* 可变缓冲区对象
 */
mutable_buffer_t *Com_MutableBuffer_Creat(void);

void Com_MutableBuffer_AddData(mutable_buffer_t *mutable_buffer,char *data,size_t data_len);

void *Com_MutableBuffer_GetData(mutable_buffer_t *mutable_buffer);

void Com_MutableBuffer_Free(mutable_buffer_t *mutable_buffer);

#endif /* __COM_UTIL_H__ */
