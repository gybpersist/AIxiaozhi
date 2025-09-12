#include "Dri_nvs.h"

nvs_handle_t my_handle;

/**
 * @brief ��ʼ�� nvs
 *
 */
void Dri_nvs_Init(void)
{
    // ��ʼ�� NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
}

/**
 * @brief ���Ҽ�
 *
 * @param key ��
 * @return esp_err_t ������Ϣ
 */
esp_err_t Dri_nvs_FindKey(char *key)
{
    esp_err_t err = nvs_find_key(my_handle, key, NULL);
    return err;
}

/**
 * @brief ���Ӽ�ֵ��
 *
 * @param key ��
 * @param value ֵ
 */
void Dri_nvs_AddKey(char *key, char *value)
{
    nvs_set_str(my_handle, key, value);
}

/**
 * @brief ��ȡ��ֵ��
 *
 * @param key ��
 * @param out_value ��ȡ����ֵ
 * @param length ֵ����
 */
void Dri_nvs_GetKey(char *key, char *out_value, size_t *length)
{
    nvs_get_str(my_handle, key, out_value, length);
}
