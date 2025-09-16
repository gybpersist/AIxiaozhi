#include "Com_Util.h"

// ���� �ɱ仺����
struct mutable_buffer
{
    char *data;
    size_t data_len;
};

/**
 * @brief ���� �ɱ仺��������
 *
 * @return mutable_buffer_t* �ɱ仺��������
 */
mutable_buffer_t *Com_MutableBuffer_Creat(void)
{
    // ���� �ɱ仺��������
    mutable_buffer_t *mutable_buffer = (mutable_buffer_t *)heap_caps_malloc(sizeof(mutable_buffer_t), MALLOC_CAP_SPIRAM);
    if (mutable_buffer != NULL)
    {
        mutable_buffer->data = NULL;
        mutable_buffer->data_len = 0;
    }

    // ���� �ɱ仺��������
    return mutable_buffer;
}

void Com_MutableBuffer_AddData(mutable_buffer_t *mutable_buffer, char *data, size_t data_len)
{
    // 1. ���������Ч�Լ�飨����Ƿ����룩
    assert(mutable_buffer);
    // �� data Ϊ NULL��data_len ����Ϊ 0�����������ݿɿ�����
    if (data == NULL && data_len > 0)
    {
        MY_LOGE("Invalid input: data is NULL but data_len > 0");
        return;
    }

    char *new_data = NULL;
    size_t new_len = mutable_buffer->data_len + data_len; // �µ���Ч�����ܳ���

    // 2. ����/�ط����ڴ�
    if (mutable_buffer->data == NULL)
    {
        // �״�ʹ�ã����� "�����ݳ��� + 1" ���ڴ�
        new_data = heap_caps_malloc(new_len + 1, MALLOC_CAP_SPIRAM);
    }
    else
    {
        // ���״Σ���չ�ڴ棨����ԭ�����ݣ��´�С = ԭ������ + ������ + 1��
        new_data = heap_caps_realloc(
            mutable_buffer->data, // ԭ���ڴ�ָ�루realloc ���Զ�����ԭ�����ݿ�����
            new_len + 1,          // ���������ڴ��С = ����Ч���� + \0
            MALLOC_CAP_SPIRAM);
    }

    // 3. �ؼ�������ڴ�����Ƿ�ɹ��������ָ�룩
    if (new_data == NULL)
    {
        // ����ʧ�ܣ�����ԭ�л��������ݣ������ǣ��������ݶ�ʧ��
        MY_LOGE("Failed to allocate SPI RAM: new_mem_size = %zu", new_len + 1);
        return;
    }

    // 4. ���������ݵ�������ĩβ�����������ݿɿ���ʱ��
    if (data_len > 0)
    {
        memcpy(
            new_data + mutable_buffer->data_len, // Ŀ���ַ��ԭ�����ݵ�ĩβ
            data,                                // Դ��ַ�������������
            data_len                             // �������ȣ����������ݵĳ���
        );
    }

    // 5. ���»�����״̬��ȷ�����������Ͱ�ȫ��
    mutable_buffer->data = new_data;      // ָ���·�����ڴ�
    mutable_buffer->data_len = new_len;   // ������Ч���ݳ���
    mutable_buffer->data[new_len] = '\0'; // ����ַ���������������Խ����ʣ�
}

void *Com_MutableBuffer_GetData(mutable_buffer_t *mutable_buffer)
{
    assert(mutable_buffer);
    return mutable_buffer->data;
}

void Com_MutableBuffer_Free(mutable_buffer_t *mutable_buffer)
{
    // ��黺����ָ����Ч��
    assert(mutable_buffer);

    // �ͷ����ݻ�������ʹ�� heap_caps_free ƥ����亯����
    if (mutable_buffer->data != NULL)
    {
        heap_caps_free(mutable_buffer->data); // ��ȷ�ͷ� SPI RAM �ڴ�
        mutable_buffer->data = NULL;          // �ÿ�ָ�룬����Ұָ��
        mutable_buffer->data_len = 0;         // ���ó���
    }

    // ���ͷ� mutable_buffer �����ɵ����߹������������ڣ�
}

char *mac_addr = NULL;
char *uuid = NULL;

/**
 * ��ȡWIFI�� mac��ַ
 */
char *Com_Get_MAC(void)
{
    // ����Ѿ���mac��ַ��ֱ�ӷ���
    if (mac_addr != NULL)
    {
        return mac_addr;
    }

    // û�оͻ�ȡ
    uint8_t eth_mac[6] = {0};
    // WIFI_IF_STA����ʾ Station ģʽ�ӿڣ��� ESP32 ��Ϊ�ͻ������ӵ����� Wi-Fi �ȵ�
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    mac_addr = heap_caps_malloc(18, MALLOC_CAP_SPIRAM);
    // "%02X:%02X:%02X:%02X:%02X:%02X",
    snprintf(mac_addr,18,
             "%02x:%02x:%02x:%02x:%02x:%02x",
             eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    return mac_addr;
}

/**
 * @brief ��ȡһ��UUID
 *
 * @return char* ��ȡ����UUID
 */
char *Com_Get_UUID(void)
{
    if (uuid != NULL)
    {
        return uuid;
    }

    uuid = heap_caps_malloc(37, MALLOC_CAP_SPIRAM);

    // ��FLASH�в�ѯUUID�Ƿ����
    if (Dri_nvs_FindKey(UUID_KEY) == ESP_OK)
    {
        size_t len = 37;
        Dri_nvs_GetKey(UUID_KEY, uuid, &len);
        return uuid;
    }

    // ���������,���������һ��UUID
    unsigned char bytes[16];
    for (int i = 0; i < 16; i++)
    {
        bytes[i] = rand() % 256;
    }

    // ���ð汾�ţ��汾4��
    bytes[6] = (bytes[6] & 0x0F) | 0x40; // ���ð汾Ϊ4
    bytes[8] = (bytes[8] & 0x3F) | 0x80; // ���ñ���Ϊ10

    // ��ʽ��ΪUUID�ַ���
    sprintf(uuid,
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            bytes[0], bytes[1], bytes[2], bytes[3],
            bytes[4], bytes[5], bytes[6], bytes[7],
            bytes[8], bytes[9], bytes[10], bytes[11],
            bytes[12], bytes[13], bytes[14], bytes[15]);

    // �����ɵ�UUID���浽FLASH��
    Dri_nvs_AddKey(UUID_KEY, uuid);

    // ���ؽ��
    return uuid;
}
