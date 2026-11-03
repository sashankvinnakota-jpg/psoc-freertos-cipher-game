/**
 * @file task_cap_touch.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief Gatekeeper task for the FT6236 capacitive touch controller
 * @version 0.1
 * @date 2026-01-07
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "task_cap_touch.h"

#if defined(ECE353_FREERTOS)
#include "rtos_events.h"

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
QueueHandle_t       Queue_CAP_TOUCH_Requests = NULL;
static SemaphoreHandle_t    I2C_Semaphore = NULL;
static cyhal_i2c_t         *I2C_Obj = NULL;
static cyhal_gpio_t         Cap_Touch_Int_Pin = NC;

/*****************************************************************************/
/* Private Task Function                                                     */
/*****************************************************************************/

/**
 * @brief Gatekeeper task for the cap touch controller.
 *        Waits for device_request_msg_t messages, reads the FT6236,
 *        and sends a device_response_msg_t back via the response queue.
 */
static void task_cap_touch(void *arg)
{
    (void)arg;

    device_request_msg_t  request;
    device_response_msg_t response;

    task_console_printf("Starting Cap Touch Task\r\n");

    while (1)
    {
        /* Wait for a request from task_console_rx */
        xQueueReceive(Queue_CAP_TOUCH_Requests, &request, portMAX_DELAY);

        /* Take the I2C bus semaphore */
        xSemaphoreTake(I2C_Semaphore, portMAX_DELAY);

        response.device = DEVICE_CAP_TOUCH;

        /* Check how many fingers are touching the screen */
        uint8_t num_touches = cap_touch_get_num_touches(I2C_Obj);

        /* Try to read XY — get_xy also validates the event flag */
        uint16_t x = 0, y = 0;
        if (num_touches > 0 && cap_touch_get_xy(I2C_Obj, &x, &y))
        {
            response.payload.cap_touch[0] = x;
            response.payload.cap_touch[1] = y;
            response.status = DEVICE_OPERATION_STATUS_READ_SUCCESS;
        }
        else
        {
            /* No valid touch detected */
            response.status = DEVICE_OPERATION_STATUS_READ_FAILURE;
        }

        /* Release the I2C bus */
        xSemaphoreGive(I2C_Semaphore);

        /* Send response back to task_console_rx */
        if (request.response_queue != NULL)
        {
            xQueueSend(request.response_queue, &response, portMAX_DELAY);
        }
    }
}

/*****************************************************************************/
/* Public Init Function                                                      */
/*****************************************************************************/

bool task_cap_touch_resources_init(
    QueueHandle_t queue_request,
    SemaphoreHandle_t i2c_semaphore,
    cyhal_i2c_t *i2c_obj,
    cyhal_gpio_t pin_cap_touch_int
)
{
    /* INT pin is optional — NC is allowed since we poll TD_STATUS directly */
    if(queue_request == NULL || i2c_semaphore == NULL || i2c_obj == NULL)
    {
        return false;
    }

    /* Save the resources */
    Queue_CAP_TOUCH_Requests = queue_request;
    I2C_Semaphore = i2c_semaphore;
    I2C_Obj = i2c_obj;
    Cap_Touch_Int_Pin = pin_cap_touch_int;

    /* Create the cap touch gatekeeper task */
    if (xTaskCreate(
            task_cap_touch,
            "Cap Touch Task",
            TASK_CAP_TOUCH_STACK_SIZE,
            NULL,
            TASK_CAP_TOUCH_PRIORITY,
            NULL) != pdPASS)
    {
        return false;
    }

    return true;
}

#endif /* ECE353_FREERTOS */
