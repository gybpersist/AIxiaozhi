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

// 定义 接收到文本数据后的回调函数
typedef void (*ws_text_cb)(const char *, int);
// 定义 接收到二进制数据后的回调函数
typedef void (*ws_bin_cb)(const char *, int);
// 定义 结束的回调函数
typedef void (*ws_finish_cb)(void);

// 创建事件标志组
extern EventGroupHandle_t ws_eventgroup;
// websocket客户端连接成功的事件标志
#define WS_CONNECTED_EVENT (1 << 1)
// 接收到HELLO消息的事件标志
#define WS_HELLO_EVENT (1 << 2)

// 定义session_id
extern char session_id[9];

/**
 * @brief 初始化 websocket客户端
 *
 * @param cb1 接收到文本数据后的回调函数
 * @param cb2 接收到二进制数据后的回调函数
 */
void Dri_websocket_Init(ws_text_cb cb1, ws_bin_cb cb2, ws_finish_cb cb3);

/**
 * @brief 开启 websocket客户端
 *
 */
void Dri_websocket_Start(void);

/**
 * @brief 关闭 websocket客户端
 *
 */
void Dri_websocket_Close(void);

/**
 * @brief 发送 hello 消息
 *
 */
void Dri_websocket_SendHello(void);

/**
 * @brief 发送 唤醒词
 *
 */
void Dri_websocket_SendWakeNet(void);

/**
 * @brief 开始 监听信息
 *
 */
void Dri_websocket_SendStartListen(void);

/**
 * @brief 停止 监听信息
 *
 */
void Dri_websocket_SendStopListen(void);

/**
 * @brief 发送 打断请求
 *
 */
void Dri_websocket_SendAbort(void);

/**
 * @brief 发送 Opus语音到服务器
 *
 * @param data  Opus语音数据
 * @param len 数据长度
 */
void Dri_websocket_SendOpusTOService(void *data, int len);

/**
 * @brief 发送需要AI控制的外设
 *
 */
void Dri_websocket_Send_iot_dev(void);

/**
 * @brief 发送需要AI控制的外设当前的状态
 *
 */
void Dri_websocket_Send_iot_dev_status(void);

#endif /* __DRI_WEBSOCKET_H__ */
