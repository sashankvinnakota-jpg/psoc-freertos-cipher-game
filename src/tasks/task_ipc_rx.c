/**
 * @file task_console_rx.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"
#include <stdbool.h>

#if defined(ECE353_FREERTOS)
#include "task_ipc.h"
#if defined(HW05)
#include "hw05.h"
#endif

/* Globals */
TaskHandle_t TaskHandle_IPC_Rx = NULL;

/* Use a double buffering strategy for IPC packets */
static volatile ipc_packet_t IPC_Rx_Buffer0;
static volatile ipc_packet_t IPC_Rx_Buffer1;

volatile ipc_packet_t* volatile IPC_Rx_Produce_Buffer = &IPC_Rx_Buffer0;
volatile ipc_packet_t* volatile IPC_Rx_Consume_Buffer = &IPC_Rx_Buffer1;

/**
 * @brief
 *
 * This task is used to process received IPC packets.  The task will block
 * on a FreeRTOS Task Notification.  When a notification is received,
 * the task will process the IPC packet stored in the consume buffer.
 *
 * For validation purposes, the task will print out the contents of the
 * received IPC packet to the console.
 * 
 * @param arg
 * Unused parameter
 */
void task_ipc_rx(void *param)
{
    while(1)
    {
        // Wait for a FreeRTOS Task Notification
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(validate_packet((ipc_packet_t *)IPC_Rx_Consume_Buffer) == true) 
        {
            // Process the received IPC packet based on command type
            switch(IPC_Rx_Consume_Buffer->cmd)
            {
                case IPC_CMD_DISCOVERY:
                    printf("Received DISCOVERY message (seq: %d)\n\r", 
                           IPC_Rx_Consume_Buffer->sequence_num);
                    // Send ACK in response to discovery
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    break;
                    
                case IPC_CMD_ACTIVE_PLAYER:
                    printf("Received ACTIVE_PLAYER message (seq: %d)\n\r", 
                           IPC_Rx_Consume_Buffer->sequence_num);
                    // Send ACK in response
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    break;
                    
                case IPC_CMD_INACTIVE_PLAYER:
                    printf("Received INACTIVE_PLAYER message (seq: %d)\n\r", 
                           IPC_Rx_Consume_Buffer->sequence_num);
                    // Send ACK in response
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    break;
                    
                case IPC_CMD_STATUS:
                    printf("Received STATUS message (seq: %d, status: 0x%02X)\n\r", 
                           IPC_Rx_Consume_Buffer->sequence_num,
                           IPC_Rx_Consume_Buffer->payload.status);
                    // Send ACK in response
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    break;
                    
                case IPC_CMD_ACK:
                    printf("Received ACK message (seq: %d)\n\r",
                           IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, ECE353_RTOS_EVENT_IPC_ACK_RECEIVED);
                    break;

#if defined(HW05)
                case IPC_CMD_CIPHER_READY:
                    /* Opponent has selected their cipher and is ready to play */
                    printf("Received CIPHER_READY (seq: %d)\n\r",
                           IPC_Rx_Consume_Buffer->sequence_num);
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, EVT_IPC_CIPHER_READY);
                    break;

                case IPC_CMD_GUESS:
                    /* Opponent sent a 4-digit guess to evaluate against our cipher */
                    printf("Received GUESS (seq: %d): %d %d %d %d\n\r",
                           IPC_Rx_Consume_Buffer->sequence_num,
                           IPC_Rx_Consume_Buffer->payload.guess.digits[0],
                           IPC_Rx_Consume_Buffer->payload.guess.digits[1],
                           IPC_Rx_Consume_Buffer->payload.guess.digits[2],
                           IPC_Rx_Consume_Buffer->payload.guess.digits[3]);
                    for (int i = 0; i < 4; i++)
                        g_opponent_guess[i] = IPC_Rx_Consume_Buffer->payload.guess.digits[i];
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, EVT_IPC_GUESS_RECEIVED);
                    break;

                case IPC_CMD_FEEDBACK:
                    /* Feedback on our last guess from the defending board */
                    printf("Received FEEDBACK (seq: %d): exact=%d near=%d pos=0x%02X\n\r",
                           IPC_Rx_Consume_Buffer->sequence_num,
                           IPC_Rx_Consume_Buffer->payload.feedback.exact,
                           IPC_Rx_Consume_Buffer->payload.feedback.near,
                           IPC_Rx_Consume_Buffer->payload.feedback.pos_result);
                    g_feedback_exact      = IPC_Rx_Consume_Buffer->payload.feedback.exact;
                    g_feedback_near       = IPC_Rx_Consume_Buffer->payload.feedback.near;
                    g_feedback_pos_result = IPC_Rx_Consume_Buffer->payload.feedback.pos_result;
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, EVT_IPC_FEEDBACK_RECEIVED);
                    break;

                case IPC_CMD_RESET:
                    /* Opponent requesting a new game */
                    printf("Received RESET (seq: %d)\n\r",
                           IPC_Rx_Consume_Buffer->sequence_num);
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, EVT_IPC_RESET_RECEIVED);
                    break;

                case IPC_CMD_FIRST_PLAYER:
                    /* Opponent's random token for turn-order negotiation */
                    printf("Received FIRST_PLAYER token=%d (seq: %d)\n\r",
                           IPC_Rx_Consume_Buffer->payload.first.token,
                           IPC_Rx_Consume_Buffer->sequence_num);
                    g_first_player_token = IPC_Rx_Consume_Buffer->payload.first.token;
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, EVT_IPC_FIRST_RECEIVED);
                    break;

                case IPC_CMD_WIN:
                    /* Opponent (the winner) sharing their final guess count */
                    printf("Received WIN guesses=%d (seq: %d)\n\r",
                           IPC_Rx_Consume_Buffer->payload.win.guesses,
                           IPC_Rx_Consume_Buffer->sequence_num);
                    g_winner_guesses = IPC_Rx_Consume_Buffer->payload.win.guesses;
                    ipc_send_ack(IPC_Rx_Consume_Buffer->sequence_num);
                    xEventGroupSetBits(ECE353_RTOS_Events, EVT_IPC_WIN_RECEIVED);
                    break;
#endif /* HW05 */

                default:
                    printf("Received UNKNOWN message type (cmd: 0x%02X, seq: %d)\n\r",
                           IPC_Rx_Consume_Buffer->cmd,
                           IPC_Rx_Consume_Buffer->sequence_num);
                    break;
            }
        }
        else {
            printf("Invalid IPC packet received!\n\r");
        }
    }
}

bool task_ipc_resources_init_rx(void)
{
    // Create the IPC Rx Task
    BaseType_t task_ipc_rx_status = xTaskCreate(
        task_ipc_rx,                 // Function that implements the task.
        "IPC Rx Task",               // Text name for the task.
        IPC_STACK_SIZE,             // Stack size in words, not bytes.
        NULL,                       // Parameter passed into the task.
        IPC_PRIORITY,               // Priority at which the task is created.
        &TaskHandle_IPC_Rx          // Used to pass out the created task's handle.
    );

    if(task_ipc_rx_status != pdPASS)
    {
        return false;
    }

    return true;    
}

#endif