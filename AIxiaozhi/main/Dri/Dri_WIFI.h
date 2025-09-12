#ifndef __DRI_WIFI_H__
#define __DRI_WIFI_H__

#include "Com_Debug.h"
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "qrcode.h" // �������ɶ�ά��

/**
 * @brief ��ʼ�� WIFI
 * 
 * @note ʹ����������
 */
void Dri_WIFI_Init(void);

#endif /* __DRI_WIFI_H__ */
