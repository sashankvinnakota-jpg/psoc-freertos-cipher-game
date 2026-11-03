/**
 * @file ice08.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE08)
#include "drivers.h"        
#include "rtos_events.h"
#include "task_buttons.h"
#include "task_lcd.h"
#include "task_joystick.h"
#include "buttons.h"

char APP_DESCRIPTION[] = "ECE353: ICE 08 - FreeRTOS LCD Gatekeeper";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
/* ADD CODE */
/* FreeRTOS Queue for LCD messages */
QueueHandle_t Queue_LCD_Request = NULL;
EventGroupHandle_t ECE353_RTOS_Events;


/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/
void task_sw1(void *pvParameters)
{
    (void)pvParameters; // Unused parameter

    printf("Starting Task SW1\n\r");

    uint32_t long_press_count = 0;
    bool pressed = false;
    uint32_t tick_count = 0; // number of 25ms ticks

    lcd_msg_request_t request;

    while(1)
    {
        // Sleep for 25 ms
        vTaskDelay(pdMS_TO_TICKS(25));

        button_state_t state = button_get_state(BUTTON_SW1);
        
        if(state != BUTTON_STATE_HIGH) {
            printf("SW1 state: %d\n\r", state);
        }

        /* detect transition to pressed */
        if(state == BUTTON_STATE_FALLING_EDGE)
        {
            printf("SW1 PRESSED\n\r");
            pressed = true;
            tick_count = 0;
        }

        if(pressed)
        {
            tick_count++;
            /* check for release */
            if(state == BUTTON_STATE_RISING_EDGE)
            {
                printf("SW1 RELEASED after %lu ticks\n\r", tick_count);
                pressed = false;
                /* if held for >= 500ms (20*25ms) count it - TODO: change to 1000ms for production */
                if(tick_count >= (500 / 25))
                {
                    long_press_count++;
                    printf("SW1 long press #%lu sent to queue\n\r", long_press_count);

                    /* send formatted message */
                    request.msg.command = LCD_CMD_PRINT_SW1_COUNT;
                    request.return_queue = NULL; /* no response needed */
                    sprintf(request.msg.payload.message,
                            "SW1 long presses: %lu", long_press_count);
                    xQueueSend(Queue_LCD_Request, &request, portMAX_DELAY);
                }
            }
        }
    }
}

void task_sw2(void *pvParameters)
{
    (void)pvParameters; // Unused parameter

    printf("Starting Task SW2\n\r");

    uint32_t long_press_count = 0;
    bool pressed = false;
    uint32_t tick_count = 0;
    lcd_msg_request_t request;

    while(1)
    {
        // Sleep for 25 ms
        vTaskDelay(pdMS_TO_TICKS(25));

        button_state_t state = button_get_state(BUTTON_SW2);
        
        if(state != BUTTON_STATE_HIGH) {
            printf("SW2 state: %d\n\r", state);
        }

        if(state == BUTTON_STATE_FALLING_EDGE)
        {
            printf("SW2 PRESSED\n\r");
            pressed = true;
            tick_count = 0;
        }

        if(pressed)
        {
            tick_count++;
            if(state == BUTTON_STATE_RISING_EDGE)
            {
                printf("SW2 RELEASED after %lu ticks\n\r", tick_count);
                pressed = false;
                if(tick_count >= (500 / 25))
                {
                    long_press_count++;
                    printf("SW2 long press #%lu sent to queue\n\r", long_press_count);
                    request.msg.command = LCD_CMD_PRINT_SW2_COUNT;
                    request.return_queue = NULL; /* no response needed */
                    sprintf(request.msg.payload.message,
                            "SW2 long presses: %lu", long_press_count);
                    xQueueSend(Queue_LCD_Request, &request, portMAX_DELAY);
                }
            }
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

    rslt = lcd_initialize();
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("LCD initialization failed!\n\r");
        for(int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    rslt = buttons_init_gpio();
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("Buttons initialization failed!\n\r");
        for(int i = 0; i < 100000; i++) {}
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
    
    /* Register the tasks with FreeRTOS*/

    ECE353_RTOS_Events = xEventGroupCreate();

    /* Create the LCD Request Queue*/
    Queue_LCD_Request = xQueueCreate(1, sizeof(lcd_msg_request_t));
    if(Queue_LCD_Request == NULL)
    {
        printf("Failed to create LCD request queue!\n\r");
        for(;;);
    }

    /* Initialize the LCD task */
    if(!task_lcd_resources_init(Queue_LCD_Request))
    {
        printf("Failed to init LCD task!\n\r");
        for(;;);
    }

    xTaskCreate(
        task_sw1, 
        "Task SW1", 
        configMINIMAL_STACK_SIZE*2, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL
    );

    xTaskCreate(
        task_sw2, 
        "Task SW2", 
        configMINIMAL_STACK_SIZE*2, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
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