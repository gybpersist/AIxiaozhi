#ifndef __COM_STATUS_H__
#define __COM_STATUS_H__

#include "Com_Debug.h"

// ALÐ¡ÖÇµÄ×´Ì¬
typedef enum
{
    Com_Status_IDLE,       // ¿ÕÏÐ×´Ì¬
    Com_Status_STRAT,      // ¿ªÊ¼×´Ì¬
    Com_Status_CONNECTION, // Á¬½Ó×´Ì¬
    Com_Status_LISTENING,  // ñöÌý×´Ì¬
    Com_Status_SPEAKERING, // Ëµ»°×´Ì¬
} Com_Status_t;

extern Com_Status_t Com_Status;

/**
 * @brief ÇÐ»» ALÐ¡ÖÇµÄµ±Ç°×´Ì¬
 * 
 * @param Status ÒªÇÐ»»µ½µÄ×´Ì¬
 */
void Com_Status_Switch(Com_Status_t Status);

#endif /* __COM_STATUS_H__ */
