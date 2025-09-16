#ifndef __DRI_HTTP_H__
#define __DRI_HTTP_H__

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "esp_crt_bundle.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"
#include "esp_app_desc.h"
#include "esp_chip_info.h"
#include "esp_partition.h"
#include "esp_psram.h"
#include "esp_ota_ops.h"

#include "Com_Util.h"
#include "cJSON.h"

/**
 * @brief http客户端初始化
 * 
 */
void Dri_http_Init(void);

#endif /* __DRI_HTTP_H__ */
