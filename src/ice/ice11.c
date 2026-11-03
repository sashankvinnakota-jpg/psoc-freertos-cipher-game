/**
 * @file ex03.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE11)
#include "drivers.h"
#include "task_buttons.h"
#include "task_ipc.h"
#include "rtos_events.h"

char APP_DESCRIPTION[] = "ECE353: ICE 11 - FreeRTOS IPC Rx/Tx";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
EventGroupHandle_t ECE353_RTOS_Events;
EventGroupHandle_t event_group;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void discover_board(uint16_t *sequence_num);
void task_system_control(void *arg);

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/* This function will be used to discover other board. This function should not
 * return until the discovery is complete.  The discovery is complete when we receive
 * a discovery message from the other board OR we send a discovery message that is 
 * Acked by the other board */
void discover_board(uint16_t *sequence_num)
{
    bool discovery_complete = false;
    
    while(discovery_complete == false)
    {
        // Send a discovery message to the other board
        if(ipc_send_discovery(*sequence_num))
        {
            printf("Sent DISCOVERY message (seq: %d)\n\r", *sequence_num);
            
            // Wait for an ACK from the other board
            if(ipc_wait_for_ack(1000))  // Wait up to 1 second for ACK
            {
                printf("Received ACK for DISCOVERY message\n\r");
                discovery_complete = true;
            }
            else
            {
                printf("No ACK received for DISCOVERY message, retrying...\n\r");
                (*sequence_num)++;
            }
        }
        else
        {
            printf("Failed to send DISCOVERY message\n\r");
        }
        
        // Small delay before retry
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    printf("Board discovery complete!\n\r");
}

/**
 * @brief
 * This task will be used to verify the functionality of the IPC UART specification
 * by ....
 *
 * @param arg 
 * Unused parameter
 */
void task_system_control(void *arg)
{
    (void)arg; // Unused parameter
    EventBits_t events;

    uint16_t sequence_num = 0;

    /* Begin the discovery process. */
    discover_board(&sequence_num);

    while(1)
    {
        // Wait for SW1, SW2,or SW3 to be pressed.  If you have not gotten task_buttons.c working yet, 
        // you will need to do so before you can proceed with this task. 
        events = xEventGroupWaitBits(ECE353_RTOS_Events,
                                    EVT_SW1 | EVT_SW2 | EVT_SW3,
                                    pdTRUE,
                                    pdFALSE,
                                    portMAX_DELAY);

        if(events & EVT_SW1)
        {
            // Increment sequence number for new message
            sequence_num++;

            /* Send the active player message  */
            if(ipc_send_active_player(sequence_num))
            {
                printf("Sent ACTIVE_PLAYER message (seq: %d)\n\r", sequence_num);
            }
            else
            {
                printf("Failed to send ACTIVE_PLAYER message\n\r");
            }

            /* Wait for the ack */
            if(ipc_wait_for_ack(5000))
            {
                printf("Received ACK for ACTIVE_PLAYER message\n\r");
            }
            else
            {
                printf("No ACK received for ACTIVE_PLAYER message\n\r");
            }
        }
        else if(events & EVT_SW2)
        {
            // Increment sequence number for new message
            sequence_num++;
            
            /* Send the inactive player message  */
            if(ipc_send_inactive_player(sequence_num))
            {
                printf("Sent INACTIVE_PLAYER message (seq: %d)\n\r", sequence_num);
            }
            else
            {
                printf("Failed to send INACTIVE_PLAYER message\n\r");
            }
            
            /* Wait for the ack */
            if(ipc_wait_for_ack(5000))
            {
                printf("Received ACK for INACTIVE_PLAYER message\n\r");
            }
            else
            {
                printf("No ACK received for INACTIVE_PLAYER message\n\r");
            }
        }
        else if(events & EVT_SW3)
        {
            // Increment sequence number for new message
            sequence_num++;

            /* Send the status message with an error code  */
            if(ipc_send_status(sequence_num, IPC_STATUS_OK))
            {
                printf("Sent STATUS message (seq: %d, status: 0x%02X)\n\r", sequence_num, IPC_STATUS_OK);
            }
            else
            {
                printf("Failed to send STATUS message\n\r");
            }

            /* Wait for the ack */
            if(ipc_wait_for_ack(5000))
            {
                printf("Received ACK for STATUS message\n\r");
            }
            else
            {
                printf("No ACK received for STATUS message\n\r");
            }
        }
        else
        {
             printf("Unknown Event!\n\r\n\r");
        }
    }
}

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void app_init_hw(void)
{
    cy_rslt_t rslt;

    console_init();
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

    // Initialize the LEDs
    rslt = leds_init();
    if(rslt != CY_RSLT_SUCCESS)
    {
        printf("LED initialization failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }

    rslt = buttons_init_gpio();

    if(rslt != CY_RSLT_SUCCESS)
    {
        printf("Button initialization failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }

}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void app_main(void)
{
    // Initialize the EventGroup
    ECE353_RTOS_Events = xEventGroupCreate();
    event_group = ECE353_RTOS_Events;

    BaseType_t button_state = xTaskCreate(
        task_buttons, 
        "Buttons", 
        configMINIMAL_STACK_SIZE*2, 
        NULL, 
        2, 
        NULL    
    );

    if(button_state != pdPASS)
    {
        printf("Button task creation failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }

    if(!task_ipc_init())
    {
        printf("IPC initialization failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }

    // Create the System Control Task
    xTaskCreate(
        task_system_control, 
        "System Control", 
        configMINIMAL_STACK_SIZE*5,      
        NULL, 
        2, 
        NULL
    );

    /* Start the scheduler*/
    vTaskStartScheduler();

    /* Will never reach this loop once the scheduler starts */
    while (1)
    {
    }
}
#endif