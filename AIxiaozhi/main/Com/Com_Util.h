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
 * ��ȡWIFI�� mac��ַ
 */
char *Com_Get_MAC(void);

/**
 * @brief ��ȡһ��UUID 
 * 
 * @return char* ��ȡ����UUID
 */
char *Com_Get_UUID(void);

// ���� �ɱ仺����
typedef struct mutable_buffer mutable_buffer_t;

/**
 * @brief ���� �ɱ仺��������
 * 
 * @return mutable_buffer_t* �ɱ仺��������
 */
mutable_buffer_t *Com_MutableBuffer_Creat(void);

void Com_MutableBuffer_AddData(mutable_buffer_t *mutable_buffer,char *data,size_t data_len);

void *Com_MutableBuffer_GetData(mutable_buffer_t *mutable_buffer);

void Com_MutableBuffer_Free(mutable_buffer_t *mutable_buffer);

#endif /* __COM_UTIL_H__ */
