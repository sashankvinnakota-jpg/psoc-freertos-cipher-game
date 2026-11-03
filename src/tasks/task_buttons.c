/**
 * @file task_buttons.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "task_buttons.h"

#ifdef ECE353_FREERTOS
#include "task_console.h"

void task_buttons(void *arg)
{
    (void)arg;

    uint8_t prev_sw1 = 1;
    uint8_t prev_sw2 = 1;
    uint8_t prev_sw3 = 1;
while(1)
{
    uint8_t curr_sw1 = (PORT_BUTTON_SW1->IN & MASK_BUTTON_SW1) ? 1 : 0;
    uint8_t curr_sw2 = (PORT_BUTTON_SW2->IN & MASK_BUTTON_SW2) ? 1 : 0;
    uint8_t curr_sw3 = (PORT_BUTTON_SW3->IN & MASK_BUTTON_SW3) ? 1 : 0;

    if((prev_sw1 == 1) && (curr_sw1 == 0))
    {
        vTaskDelay(pdMS_TO_TICKS(30));
        if((PORT_BUTTON_SW1->IN & MASK_BUTTON_SW1) == 0)
            xEventGroupSetBits(event_group, EVT_SW1);
    }

    if((prev_sw2 == 1) && (curr_sw2 == 0))
    {
        vTaskDelay(pdMS_TO_TICKS(30));
        if((PORT_BUTTON_SW2->IN & MASK_BUTTON_SW2) == 0)
            xEventGroupSetBits(event_group, EVT_SW2);
    }

    if((prev_sw3 == 1) && (curr_sw3 == 0))
    {
        vTaskDelay(pdMS_TO_TICKS(30));
        if((PORT_BUTTON_SW3->IN & MASK_BUTTON_SW3) == 0)
            xEventGroupSetBits(event_group, EVT_SW3);
    }

    // update prev AFTER all checks, no manual override inside if blocks
    prev_sw1 = curr_sw1;
    prev_sw2 = curr_sw2;
    prev_sw3 = curr_sw3;

    vTaskDelay(pdMS_TO_TICKS(10));
}
}
    
#endif