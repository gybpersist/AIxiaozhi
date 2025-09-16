#include "Com_status.h"

Com_Status_t Com_Status = Com_Status_IDLE;

char *Status_arr[5] = {
    "Com_Status_IDLE",       // ����״̬
    "Com_Status_STRAT",      // ��ʼ״̬
    "Com_Status_CONNECTION", // ����״̬
    "Com_Status_LISTENING",  // ����״̬
    "Com_Status_SPEAKERING", // ˵��״̬
};

/**
 * @brief �л� ALС�ǵĵ�ǰ״̬
 *
 * @param Status Ҫ�л�����״̬
 */
void Com_Status_Switch(Com_Status_t Status)
{
    if (Com_Status != Status)
    {
        MY_LOGE("xiaozhi switch status %s ---> %s\r\n", Status_arr[Com_Status], Status_arr[Status]);
        Com_Status = Status;
    }
}
