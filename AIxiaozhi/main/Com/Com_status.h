#ifndef __COM_STATUS_H__
#define __COM_STATUS_H__

#include "Com_Debug.h"

// ALС�ǵ�״̬
typedef enum
{
    Com_Status_IDLE,       // ����״̬
    Com_Status_STRAT,      // ��ʼ״̬
    Com_Status_CONNECTION, // ����״̬
    Com_Status_LISTENING,  // ����״̬
    Com_Status_SPEAKERING, // ˵��״̬
} Com_Status_t;

extern Com_Status_t Com_Status;

/**
 * @brief �л� ALС�ǵĵ�ǰ״̬
 * 
 * @param Status Ҫ�л�����״̬
 */
void Com_Status_Switch(Com_Status_t Status);

#endif /* __COM_STATUS_H__ */
