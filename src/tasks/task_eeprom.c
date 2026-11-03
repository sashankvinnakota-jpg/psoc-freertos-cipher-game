/**
 * @file task_eeprom.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-09-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ECE353_FREERTOS)
#include "drivers.h"
 #include "task_eeprom.h"
 #include "task_console.h"

 /* Global Variables */
QueueHandle_t Queue_EEPROM_Requests;
static SemaphoreHandle_t *SPI_Semaphore = NULL;
static cyhal_spi_t *eeprom_spi_obj = NULL;
static cyhal_gpio_t eeprom_cs_pin = NC;

/**
 * @brief 
 *  This function acts as the published interface to write data to the EEPROM.
 *  It will format the device request message and send it to the EEPROM task.
 *
 *  If the user provides a valid return queue, this function will wait for
 *  the response from the EEPROM task before returning.  If the user provides
 *  a NULL return queue, this function will return immediately after sending
 *  the request.
 * @param return_queue 
 * @param address 
 * @param data 
 * @param length 
 * @return true 
 * @return false 
 */
bool system_sensors_eeprom_write(QueueHandle_t return_queue, uint16_t address, uint8_t data)
{
    device_request_msg_t request;
    device_response_msg_t response;

    if(Queue_EEPROM_Requests == NULL)
    {
        return false;
    }

    request.device = DEVICE_EEPROM;
    request.operation = DEVICE_OP_WRITE;
    request.address = address;
    request.value = data;
    request.response_queue = return_queue;

    if(xQueueSend(Queue_EEPROM_Requests, &request, portMAX_DELAY) != pdPASS)
    {
        return false;
    }

    if(return_queue != NULL)
    {
        if(xQueueReceive(return_queue, &response, portMAX_DELAY) != pdPASS)
        {
            return false;
        }

        if(response.device != DEVICE_EEPROM)
        {
            return false;
        }

        if(response.status != DEVICE_OPERATION_STATUS_WRITE_SUCCESS)
        {
            return false;
        }
    }

    return true;
}   

/**
 * @brief 
 * This function is the published interface to read data from the EEPROM.
 * It will format the device request message and send it to the EEPROM task.
 *
 * The value read from the EEPROM will be returned via the data pointer
 *
 * @param return_queue 
 * @param address 
 * @param data 
 * @return true 
 * @return false 
 */
bool system_sensors_eeprom_read(QueueHandle_t return_queue, uint16_t address, uint8_t *data)
{
    device_request_msg_t request;
    device_response_msg_t response;

    if(return_queue == NULL || data == NULL)
    {
        return false;
    }

    if(Queue_EEPROM_Requests == NULL)
    {
        return false;
    }

    request.device = DEVICE_EEPROM;
    request.operation = DEVICE_OP_READ;
    request.address = address;
    request.value = 0;
    request.response_queue = return_queue;

    if(xQueueSend(Queue_EEPROM_Requests, &request, portMAX_DELAY) != pdPASS)
    {
        return false;
    }

    if(xQueueReceive(return_queue, &response, portMAX_DELAY) != pdPASS)
    {
        return false;
    }

    if(response.device != DEVICE_EEPROM)
    {
        return false;
    }

    if(response.status != DEVICE_OPERATION_STATUS_READ_SUCCESS)
    {
        return false;
    }

    *data = response.payload.eeprom;

    return true;
}

/**
 * @brief 
 *  Task used to monitor the reception of command packets sent the EEPROM
 * @param arg 
 */
void task_eeprom(void *arg)
 {
    (void) arg;
    device_request_msg_t request_packet;
    device_response_msg_t response_packet;

    task_console_printf("Starting EEPROM Task\r\n");

    while(1)
    {
        // Wait for a request to arrive
        xQueueReceive(
            Queue_EEPROM_Requests, 
            &request_packet, 
            portMAX_DELAY
        );

        // take spi bus sempahore
        xSemaphoreTake(*SPI_Semaphore, portMAX_DELAY);

        response_packet.device = DEVICE_EEPROM;

        // perform the requested action (read or write)
        switch(request_packet.operation)
        {
            case DEVICE_OP_WRITE:
                eeprom_write_byte(eeprom_spi_obj, eeprom_cs_pin, request_packet.address, request_packet.value);
                response_packet.status = DEVICE_OPERATION_STATUS_WRITE_SUCCESS;
                break;

            case DEVICE_OP_READ:
                response_packet.payload.eeprom = eeprom_read_byte(eeprom_spi_obj, eeprom_cs_pin, request_packet.address);
                response_packet.status = DEVICE_OPERATION_STATUS_READ_SUCCESS;
                break;

            default:
                // Invalid request type.
                response_packet.status = DEVICE_OPERATION_STATUS_READ_FAILURE;
                break;
        }

        // give spi bus semaphore
        xSemaphoreGive(*SPI_Semaphore);

        // Return data when a response queue is provided.
        if(request_packet.response_queue != NULL)
        {
            xQueueSend(request_packet.response_queue, &response_packet, portMAX_DELAY);
        }
    }
}


/**
 * @brief 
 * Function used to initialize resources for the EEPROM task
 * @param spi_semaphore 
 * @param spi_obj 
 * @param cs_pin 
 * @return true 
 * @return false 
 */
bool task_eeprom_resources_init(SemaphoreHandle_t *spi_semaphore, cyhal_spi_t *spi_obj, cyhal_gpio_t cs_pin)
 {
    if(spi_semaphore == NULL || spi_obj == NULL || cs_pin == NC)
    {
        return false;
    }

    // Save handles for access to the SPI peripheral and semaphore
    SPI_Semaphore = spi_semaphore;
    eeprom_spi_obj = spi_obj;
    eeprom_cs_pin = cs_pin;

    /*Create the EEPROM Requests Queue */
    Queue_EEPROM_Requests = xQueueCreate(1, sizeof(device_request_msg_t ));

    /* Create the FreeRTOS task for the EEPROM */
    if (xTaskCreate(
        task_eeprom, 
        "EEPROM", 
        10*configMINIMAL_STACK_SIZE, 
        spi_semaphore, 
        tskIDLE_PRIORITY + 1, 
        NULL) != pdPASS)
    {
        return false;
    }
    return true;
 }

#endif /* ECE353_FREERTOS */