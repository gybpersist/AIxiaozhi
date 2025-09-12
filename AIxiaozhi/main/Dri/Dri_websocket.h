#ifndef __DRI_WEBSOCKET_H__
#define __DRI_WEBSOCKET_H__

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_crt_bundle.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include <cJSON.h>
#include "Com_Util.h"

#define WS_URL "wss://api.tenclass.net/xiaozhi/v1/"
#define ACCESS_TOKEN "test-token"

// ���� ���յ��ı����ݺ�Ļص�����
typedef void (*ws_text_cb)(const char *, int);
// ���� ���յ����������ݺ�Ļص�����
typedef void (*ws_bin_cb)(const char *, int);
// ���� �����Ļص�����
typedef void (*ws_finish_cb)(void);

// �����¼���־��
extern EventGroupHandle_t ws_eventgroup;
// websocket�ͻ������ӳɹ����¼���־
#define WS_CONNECTED_EVENT (1 << 1)
// ���յ�HELLO��Ϣ���¼���־
#define WS_HELLO_EVENT (1 << 2)

// ����session_id
extern char session_id[9];

/**
 * @brief ��ʼ�� websocket�ͻ���
 *
 * @param cb1 ���յ��ı����ݺ�Ļص�����
 * @param cb2 ���յ����������ݺ�Ļص�����
 */
void Dri_websocket_Init(ws_text_cb cb1, ws_bin_cb cb2,ws_finish_cb cb3);

/**
 * @brief ���� websocket�ͻ���
 *
 */
void Dri_websocket_Start(void);

/**
 * @brief �ر� websocket�ͻ���
 *
 */
void Dri_websocket_Close(void);

/**
 * @brief ���� hello ��Ϣ
 *
 */
void Dri_websocket_SendHello(void);

/**
 * @brief ���� ���Ѵ�
 * 
 */
void Dri_websocket_SendWakeNet(void);

/**
 * @brief ��ʼ ������Ϣ
 * 
 */
void Dri_websocket_SendStartListen(void);

/**
 * @brief ֹͣ ������Ϣ
 * 
 */
void Dri_websocket_SendStopListen(void);

/**
 * @brief ���� �������
 * 
 */
void Dri_websocket_SendAbort(void);

/**
 * @brief ���� Opus������������
 * 
 * @param data  Opus��������
 * @param len ���ݳ���
 */
void Dri_websocket_SendOpusTOService(void *data, int len);

#endif /* __DRI_WEBSOCKET_H__ */
