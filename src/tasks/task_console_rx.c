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

#ifdef ECE353_FREERTOS
#include "drivers.h"
#include "task_console.h"
#include "cyhal_uart.h"
#include "leds.h"
#include <string.h>
#include "devices.h"
#include "task_eeprom.h"
#include "task_cap_touch.h"

/**
 * @brief
 * This file contains the implementation of the console receive (Rx) task.
 * The task is responsible for processing incoming console commands and
 * controlling the state of the LEDs accordingly.
 * 
 * The task uses a double buffer to process the incoming console commands.
 * The supported commands will be "RED_ON" and "RED_OFF" to control the red LED.
 */

/* ADD CODE */
/* Global Variables */
console_buffer_t console_buffer1;
console_buffer_t console_buffer2;
// allocate pointers to the buffers
console_buffer_t *produce_console_buffer;
console_buffer_t *consume_console_buffer;

// allocate a task handle
TaskHandle_t TaskHandle_Console_Rx;

/**
 * @brief
 * This function is the bottom half task for receiving console input.
 *
 * It waits for a task notification from the ISR indicating that a new 
 * command has been received. The task then processes the command and 
 * controls the state of the LEDs accordingly.
 *
 * @param param Unused parameter
 */
void task_console_rx(void *param)
{
     device_request_msg_t request;
    device_response_msg_t response;

    /* Create a local queue to receive responses back from gatekeeper tasks */
    QueueHandle_t response_queue = xQueueCreate(1, sizeof(device_response_msg_t));

    (void)param;
    while (1)
    {
       /* Wait for a new command from the console ISR */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Parse the command string into a request struct */
        if (!parse_cli_data(consume_console_buffer->data, &request))
        {
            continue; /* Invalid command, ignore it */
        }

        /* Attach our response queue so the gatekeeper knows where to reply */
        request.response_queue = response_queue;

        /* Send to the right gatekeeper based on which device was parsed */
      if (request.device == DEVICE_EEPROM)
{
    xQueueSend(Queue_EEPROM_Requests, &request, portMAX_DELAY);
    xQueueReceive(response_queue, &response, portMAX_DELAY);

    if (request.operation == DEVICE_OP_WRITE)
        task_console_printf("EEPROM WRITE: Addr=0x%04X, Value=0x%02X\r\n",
            request.address, request.value);
    else
        task_console_printf("EEPROM READ: Addr=0x%04X, Value=0x%02X\r\n",
            request.address, response.payload.eeprom);
}
else if (request.device == DEVICE_CAP_TOUCH)
{
    xQueueSend(Queue_CAP_TOUCH_Requests, &request, portMAX_DELAY);      
    xQueueReceive(response_queue, &response, portMAX_DELAY);
if (response.status == DEVICE_OPERATION_STATUS_READ_SUCCESS)
    {
        uint16_t raw_s0 = response.payload.cap_touch[0];
        uint16_t raw_s1 = response.payload.cap_touch[1];
        uint16_t screen_x = (raw_s1 > 17)  ? ((raw_s1 - 17)  * 319 / 292) : 0;
        uint16_t screen_y = (raw_s0 < 229) ? ((229 - raw_s0) * 239 / 219) : 0;
        if (screen_x > 319) screen_x = 319;
        if (screen_y > 239) screen_y = 239;
        task_console_printf("Cap Touch: X=%d, Y=%d\r\n", screen_x, screen_y);
    }
    else
        task_console_printf("Failed to read Capacitive Touch data\r\n");}
}

}
/**
 * @brief
 * This function initializes the resources for the console Rx task.
 * @return true if resources were initialized successfully
 * @return false if resource initialization failed
 */
bool task_console_resources_init_rx(void)
{
    BaseType_t rslt;

    console_buffer1.data = (char *)pvPortMalloc(CONSOLE_MAX_MESSAGE_LENGTH);
    console_buffer2.data = (char *)pvPortMalloc(CONSOLE_MAX_MESSAGE_LENGTH);


    if (console_buffer1.data == NULL || console_buffer2.data == NULL)
    {
        return false;
    }


    memset(console_buffer1.data, 0, CONSOLE_MAX_MESSAGE_LENGTH);
    memset(console_buffer2.data, 0, CONSOLE_MAX_MESSAGE_LENGTH);

    produce_console_buffer = &console_buffer1;
    consume_console_buffer = &console_buffer2;
    produce_console_buffer->index = 0;
    consume_console_buffer->index = 0;

    rslt = xTaskCreate(
        task_console_rx,
        "Console Rx",
        (configMINIMAL_STACK_SIZE)*5,
        NULL,
        tskIDLE_PRIORITY + 1,
        &TaskHandle_Console_Rx
    );

    return (rslt == pdPASS);
}

#endif