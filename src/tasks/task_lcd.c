/**
 * @file task_lcd.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "main.h"
 #include "task_lcd.h"  
 #include "lcd-io.h"    
 #include "task_console.h"
 #include "master_mind_lib.h"           


#if defined(ECE353_FREERTOS)

/* FreeRTOS Queue for LCD messages */
static QueueHandle_t Queue_Requests = NULL;

/* LCD Task */
void task_lcd(void *pvParameters)
{
    (void)pvParameters; // Unused parameter

    lcd_msg_request_t lcd_request;
    lcd_msg_response_t response;
    bool status = false;

    /* Optional debug message */
    //task_console_printf("Starting LCD Task\r\n");

    while(1)
    {
        /* Block indefinitely until a request packet arrives */
        if(xQueueReceive(Queue_Requests, &lcd_request, portMAX_DELAY) == pdTRUE)
        {
            printf("LCD Task: received command %d\r\n", lcd_request.msg.command);
            status = false;
            switch(lcd_request.msg.command)
            {
                case LCD_CMD_DRAW_TILE:
                case LCD_CMD_DRAW_TILE_INVERTED:
                case LCD_CMD_PRINT_MESSAGE:
                  case LCD_CMD_CLEAR_SCREEN:
    status = master_mind_handle_msg(&lcd_request.msg);
    break;
                case LCD_CMD_PRINT_SW1_COUNT:
                    /* print the received string to console */
                    printf("LCD SW1: %s\r\n", lcd_request.msg.payload.message);
                    /* draw the string on the LCD starting at (10,50) */
                    lcd_draw_string(10, 50, lcd_request.msg.payload.message,
                                    LCD_COLOR_WHITE, LCD_COLOR_BLACK);
                    status = true;
                    break;

                case LCD_CMD_PRINT_SW2_COUNT:
                    printf("%s\r\n", lcd_request.msg.payload.message);
                    lcd_draw_string(10, 100, lcd_request.msg.payload.message,
                                    LCD_COLOR_WHITE, LCD_COLOR_BLACK);
                    status = true;
                    break;

                default:
                    /* unhandled command - ignore */
                    status = false;
                    break;
            }

            /* send response back if caller provided a queue */
            if(lcd_request.return_queue != NULL)
            {
                response = status ? LCD_MSG_RESPONSE_SUCCESS : LCD_MSG_RESPONSE_ERROR;
                xQueueSend(lcd_request.return_queue, &response, 0);
            }
        }
    }
}

/* LCD Task Initialization */
bool task_lcd_resources_init(QueueHandle_t queue_request){

    BaseType_t result;

    if (queue_request == NULL)
    {
        return false;
    }
    Queue_Requests = queue_request;

    /* Create the LCD Task */
    result= xTaskCreate(
        task_lcd,                       // Task function
        "LCD Task",                     // Task name
        TASK_LCD_STACK_SIZE,            // Stack size
        NULL,                           // Task parameters
        TASK_LCD_PRIORITY,              // Task priority
        NULL                            // Task handle
    );

    if(result != pdPASS)
    {
        return false;
    }   

    return true;
}
#endif