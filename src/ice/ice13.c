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
#include "cyhal_hw_types.h"
#include "main.h"

#if defined(ICE13)
#include "drivers.h"
#include "task_console.h"
#include "task_light_sensor.h"

char APP_DESCRIPTION[] = "ECE353: ICE 13 - FreeRTOS Ambient Light Sensor";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
cyhal_i2c_t *I2C_Obj = NULL;
SemaphoreHandle_t I2C_Semaphore = NULL;
QueueHandle_t Queue_Light_Sensor_Responses = NULL;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void task_system_control(void *arg);

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief 
 * This function will initialize all of the software resources for the 
 * System Control Task
 * @return true 
 * @return false 
 */
bool task_system_control_resources_init(void)
{
    /* Create a Queue for the temp sensor task */
    Queue_Light_Sensor_Responses = xQueueCreate(1, sizeof(device_response_msg_t));

    /* Create the I2C Semaphore */
    I2C_Semaphore = xSemaphoreCreateMutex();
    if (I2C_Semaphore == NULL)
    {
        return false;
    }

    /* Create the System Control Task */
    if (xTaskCreate(
            task_system_control,
            "System Control Task",
            configMINIMAL_STACK_SIZE*5,
            NULL,
            tskIDLE_PRIORITY + 1,
            NULL) != pdPASS)
    {
        return false;
    }

    return true;
}   

void task_system_control(void *arg)
{
    (void)arg; // Unused parameter
    device_request_msg_t request_packet;
    device_response_msg_t response_packet;
    
    task_console_printf("Starting System Control Task\r\n");

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));

        // Send read request to light sensor gatekeeper task.
        request_packet.device = DEVICE_LIGHT;
        request_packet.operation = DEVICE_OP_READ;
        request_packet.response_queue = Queue_Light_Sensor_Responses;

        if(xQueueSend(Queue_Light_Sensor_Requests, &request_packet, portMAX_DELAY) != pdPASS)
        {
            task_console_printf("Ambient Light request send failed!\r\n");
            continue;
        }

        if(xQueueReceive(Queue_Light_Sensor_Responses, &response_packet, portMAX_DELAY) != pdPASS)
        {
            task_console_printf("Ambient Light response receive failed!\r\n");
            continue;
        }

        if((response_packet.device == DEVICE_LIGHT) &&
           (response_packet.status == DEVICE_OPERATION_STATUS_READ_SUCCESS))
        {
            task_console_printf("Ambient Light CH0: %u\r\n", response_packet.payload.light_sensor);
        }
        else
        {
            task_console_printf("Ambient Light read failed!\r\n");
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
    console_init();
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

    /* Initialize the i2c interface */
    I2C_Obj = i2c_init(PIN_I2C_SDA, PIN_I2C_SCL);
    if (I2C_Obj == NULL)
    {
        printf("I2C initialization failed!\n\r");
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
    if(!task_system_control_resources_init())
    {
        printf("System Control Task initialization failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }

    if(!task_console_init())
    {
        printf("Console initialization failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }

    if(!task_light_sensor_resources_init(&I2C_Semaphore, I2C_Obj))
    {
        printf("Light Sensor Task initialization failed!\n\r");
        for(int i = 0; i < 10000; i++);
        CY_ASSERT(0);
    }   

    /* Start the scheduler*/
    vTaskStartScheduler();

    /* Will never reach this loop once the scheduler starts */
    while (1)
    {
    }
}
#endif