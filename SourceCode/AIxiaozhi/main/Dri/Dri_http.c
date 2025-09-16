#include "Dri_http.h"

static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    // �ѻ�õ����ݷŵ��ɱ仺��������
    mutable_buffer_t *mutable_buffer = (mutable_buffer_t *)evt->user_data;

    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        // ���Ͻ�������
        Com_MutableBuffer_AddData(mutable_buffer, (char *)evt->data, (size_t)evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        // ���յ�������
        MY_LOGE("receivedata: %s", (char *)Com_MutableBuffer_GetData(mutable_buffer));
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        esp_http_client_set_header(evt->client, "From", "user@example.com");
        esp_http_client_set_header(evt->client, "Accept", "text/html");
        esp_http_client_set_redirection(evt->client);
        break;
    }
    return ESP_OK;
}

/**
 * @brief ���������庯�� (cJSON��ʽ)
 *
 * @return char* ������(cJSON��ʽ)
 */
static char *Dri_http_Set_Body()
{
    // ���� cJSON����
    cJSON *root = cJSON_CreateObject();

    // ���� application����
    cJSON *application_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(application_obj, "version", esp_get_idf_version());
    MY_LOGE("elf_sga256: %s",esp_app_get_elf_sha256_str());
    cJSON_AddStringToObject(application_obj, "elf_sha256", esp_app_get_elf_sha256_str());
    cJSON_AddItemToObject(root, "application", application_obj);

    // ���� board����
    cJSON *board_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(board_obj, "type", "bread-compact-wifi");
    cJSON_AddStringToObject(board_obj, "name", "bread-compact-wifi");
    cJSON_AddStringToObject(board_obj, "ssid", "Lenovo Y70");
    cJSON_AddNumberToObject(board_obj, "rssi", -55);
    cJSON_AddItemToObject(root, "board", board_obj);

    // ��cjsonתΪ�ַ���
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

/**
 * @brief http�ͻ��˳�ʼ��
 *
 */
void Dri_http_Init(void)
{
    // ���� �ɱ仺�������ڽ�����Ӧ����
    mutable_buffer_t *mutable_buffer = Com_MutableBuffer_Creat();

    // http������Ϣ
    esp_http_client_config_t config = {
        .url = "https://api.tenclass.net/xiaozhi/ota/", // �������� URL
        .event_handler = _http_event_handler,           // �¼��ص�����
        .method = HTTP_METHOD_POST,                     // http post����
        .user_data = mutable_buffer,                    // ���ݸ��ص����û�����
        .crt_bundle_attach = esp_crt_bundle_attach,     // HTTPS ֤������
        .timeout_ms = 15000,                            // ��ʱʱ��
        .disable_auto_redirect = true,                  // �����Զ��ض���
    };
    // ��ʼ�� HTTP �ͻ���
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // �������ͷ
    esp_http_client_set_header(client, "Device-Id", Com_Get_MAC());
    esp_http_client_set_header(client, "Client-Id", Com_Get_UUID());
    esp_http_client_set_header(client, "User-Agent", "bread-compact-wifi/1.0.0");
    esp_http_client_set_header(client, "Accept-Language", "zh-CN");
    MY_LOGE("mac: %s,uuid: %s\r\n",Com_Get_MAC(),Com_Get_UUID());

    // ���������
    char *body_json = Dri_http_Set_Body(); // ���͵� JSON ��ʽ����
    MY_LOGE("body_json: %s", body_json);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, body_json, strlen(body_json));

    // �������� 
    esp_http_client_perform(client);

    int status = esp_http_client_get_status_code(client);
    if (status == 200)
    {
        MY_LOGE("http client is OK...\r\n");
    }
    else
    {
        MY_LOGE("http client status is %d...\r\n",status);
    }

    // �ͷ���Դ
    free(body_json);
    Com_MutableBuffer_Free(mutable_buffer);
    esp_http_client_cleanup(client);
}

// {
//   "mqtt": {
//     "endpoint": "mqtt.xiaozhi.me",
//     "client_id": "GID_test@@@24_EC_4A_25_0D_30@@@2dcf4629-04b4-48d8-a8a7-ff3f2bf1fcd9",
//     "username": "eyJpcCI6IjIyMy4xNjAuMTE5Ljk5In0=",
//     "password": "Pdlt+b450ukn5SQ9O8sOqCmmPtOOZB49aqds8fTji7g=",
//     "publish_topic": "device-server",
//     "subscribe_topic": "null"
//   },
//   "websocket": {
//     "url": "wss://api.tenclass.net/xiaozhi/v1/",
//     "token": "test-token"
//   },
//   "server_time": {
//     "timestamp": 1757555183540,
//     "timezone_offset": 480
//   },
//   "firmware": {
//     "version": "v5.3.1-dirty",
//     "url": ""
//   },
//   "activation": {
//     "code": "448902",
//     "message": "xiaozhi.me\n448902",
//     "challenge": "3c1827af-9d85-4659-8f6d-0284802d8f83"
//   }
// }