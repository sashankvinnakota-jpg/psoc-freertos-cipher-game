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

#if defined(ICE05)
#include "drivers.h"
#include "rtos_events.h"
#include "task_buttons.h"
#include "task_buzzer.h"

char APP_DESCRIPTION[] = "ECE353: ICE 05 - FreeRTOS Event Groups";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/
// ADD CODE for Event Group Bit Definitions

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
// ADD CODE for Event Group Handle
EventGroupHandle_t event_group;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

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

    /* ADD CODE Initialize the buttons */
    buttons_init_gpio();

    /* Initialize Buzzer (50% duty cycle, 2kHz) */
    rslt = buzzer_init(0.5, 2000);
    CY_ASSERT(rslt == CY_RSLT_SUCCESS);

    /* ADD CODE Initialize the buzzer */
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
    event_group = xEventGroupCreate();

    CY_ASSERT(event_group != NULL);

    /* Create Tasks */
    xTaskCreate(
        task_buttons,
        "Buttons Task",
        1024,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        task_buzzer,
        "Buzzer Task",
        1024,
        NULL,
        1,
        NULL
    );

    /* Start Scheduler */
    vTaskStartScheduler();
    while (1)
    {
    }
}
#endif