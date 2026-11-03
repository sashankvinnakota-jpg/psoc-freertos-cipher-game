/**
 * @file rtos_events.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef __RTOS_EVENTS_H__
 #define __RTOS_EVENTS_H__
 #include "main.h"
 
#ifdef ECE353_FREERTOS



/*******************************************************************************
* Event Group for system events.
 ******************************************************************************/
extern EventGroupHandle_t ECE353_RTOS_Events;

/*******************************************************************************
* Macros used to define the system events
******************************************************************************/
#define EVT_SW1   (1 << 0)
#define EVT_SW2   (1 << 1)
#define EVT_SW3   (1 << 2)
#define EVT_JOYSTICK_LEFT   (1 << 3)
#define EVT_JOYSTICK_RIGHT  (1 << 4)
#define EVT_JOYSTICK_UP     (1 << 5)
#define EVT_JOYSTICK_DOWN   (1 << 6)
#define ECE353_RTOS_EVENT_IPC_ACK_RECEIVED (1 << 7)
extern EventGroupHandle_t event_group;


#endif // ECE353_FREERTOS

#endif // __RTOS_EVENTS_H__