/**
 * @file task_joystick.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "main.h"

#ifdef ECE353_FREERTOS  
#include "drivers.h"
 #include "task_joystick.h"

 QueueHandle_t Queue_Joystick = NULL;

/* Message lookup table for joystick positions */
const char * const joystick_pos_names[] = {
    "Center",
    "Left",
    "Right",
    "Up",
    "Down",
    "Upper Left",
    "Upper Right",
    "Lower Left",
    "Lower Right"
};

 /**
  * @brief 
  *  Task used to monitor the joystick
  * @param arg 
  */
void task_joystick(void *arg)
{
    (void)arg;
    uint16_t x_value, y_value;
    float x_voltage, y_voltage;
    static joystick_position_t previous_pos = JOYSTICK_POS_CENTER;
    joystick_position_t current_pos;

    while(1)
    {
        x_value = joystick_read_x();
        y_value = joystick_read_y();

        x_voltage = (x_value / 65535.0f) * 3.3f;
        y_voltage = (y_value / 65535.0f) * 3.3f;

        /* Determine cardinal direction only, collapse diagonals to center */
        if (x_voltage > 2.475f) {
            current_pos = JOYSTICK_POS_LEFT;
        } else if (x_voltage < 0.825f) {
            current_pos = JOYSTICK_POS_RIGHT;
        } else if (y_voltage > 2.475f) {
            current_pos = JOYSTICK_POS_UP;
        } else if (y_voltage < 0.825f) {
            current_pos = JOYSTICK_POS_DOWN;
        } else {
            current_pos = JOYSTICK_POS_CENTER;
        }

        /* Only fire event on fresh deflection from center */
        if (previous_pos == JOYSTICK_POS_CENTER && current_pos != JOYSTICK_POS_CENTER)
        {
            switch(current_pos)
            {
                case JOYSTICK_POS_LEFT:  xEventGroupSetBits(event_group,EVT_JOYSTICK_LEFT); 
                printf("Joystick LEFT\r\n");
                 break;
                case JOYSTICK_POS_RIGHT: xEventGroupSetBits(event_group,EVT_JOYSTICK_RIGHT ); 
                printf("Joystick RIGHT\r\n");
                break;
                case JOYSTICK_POS_UP:    xEventGroupSetBits(event_group,EVT_JOYSTICK_UP );    
                printf("Joystick UP\r\n");
                break;
                case JOYSTICK_POS_DOWN:  xEventGroupSetBits(event_group,EVT_JOYSTICK_DOWN );  
                printf("Joystick DOWN\r\n");
                break;
                default: break;
            }
        }

        previous_pos = current_pos;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
bool task_joystick_init(void)
{
    /* Create the Queue used to send Joystick Positions*/
 Queue_Joystick = xQueueCreate(
                        1,                                  // Queue length
                        sizeof(joystick_position_t)         // Item size
                     );

    /* Create the joystick task */
    xTaskCreate(
            task_joystick,          // Task function
            "Task Joystick",        // Task name
            configMINIMAL_STACK_SIZE,
            NULL,
            1,
            NULL) ;
    
     
    

    
    return true;
}
#endif