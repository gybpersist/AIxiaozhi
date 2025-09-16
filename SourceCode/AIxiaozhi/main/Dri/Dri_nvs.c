#include "Dri_nvs.h"

nvs_handle_t my_handle;

/**
 * @brief 初始化 nvs
 *
 */
void Dri_nvs_Init(void)
{
    // 初始化 NVS
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
 * @brief 查找键
 *
 * @param key 键
 * @return esp_err_t 错误信息
 */
esp_err_t Dri_nvs_FindKey(char *key)
{
    esp_err_t err = nvs_find_key(my_handle, key, NULL);
    return err;
}

/**
 * @brief 增加键值对
 *
 * @param key 键
 * @param value 值
 */
void Dri_nvs_AddKey(char *key, char *value)
{
    nvs_set_str(my_handle, key, value);
}

/**
 * @brief 获取键值对
 *
 * @param key 键
 * @param out_value 获取键的值
 * @param length 值长度
 */
void Dri_nvs_GetKey(char *key, char *out_value, size_t *length)
{
    nvs_get_str(my_handle, key, out_value, length);
}
