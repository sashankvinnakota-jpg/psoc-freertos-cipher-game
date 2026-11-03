/**
 * @file devices.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-10-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "devices.h"

#if defined(ECE353_FREERTOS)
#include "task_imu.h"
#include "task_temp_sensor.h"
#include "task_light_sensor.h"
#include "task_io_expander.h"
#include "task_eeprom.h"
#include "task_console.h"
bool parse_cli_data(char *data, device_request_msg_t *request)
{
    char device[16] = {0};
    char op[8]      = {0};

    /* Skip any leading non-printable characters (e.g. 0x03 from serial monitor) */
    while (*data && (*data < 0x20 || *data > 0x7E)) data++;

    /* Pull first word to figure out which device */
    sscanf(data, "%s", device);

    if (strcmp(device, "EEPROM") == 0)
    {
        /* Pull op and address */
        sscanf(data, "%s %s %hx", device, op, &request->address);

        request->device = DEVICE_EEPROM;
        request->response_queue = NULL;

        if (strcmp(op, "R") == 0)
        {
            request->operation = DEVICE_OP_READ;
            request->value = 0;
            return true;
        }
        else if (strcmp(op, "W") == 0)
        {
            /* %hhx not reliable on newlib-nano, use unsigned int then cast */
            unsigned int value_tmp = 0;
            sscanf(data, "%s %s %hx %x", device, op, &request->address, &value_tmp);
            request->value = (uint8_t)value_tmp;
            request->operation = DEVICE_OP_WRITE;
            return true;
        }
    }
    else if (strcmp(device, "CAP_TOUCH") == 0)
    {
        request->device = DEVICE_CAP_TOUCH;
        request->operation = DEVICE_OP_READ;
        request->response_queue = NULL;
        return true;
    }

    return false;
}

#endif /* ECE353_FREERTOS */
