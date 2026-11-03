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

#if defined(EX05)

#include "drivers.h"

char APP_DESCRIPTION[] = "ECE353: Example 05 - FreeRTOS Tasks";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
volatile bool buzzer_enable=false;
/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void task_button_sw1(void *arg);
void task_button_sw2(void *arg);
void task_buzzer(void *arg);

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/
void task_button_sw1(void *arg){
    (void)arg;
   uint32_t button_count=0;
printf("SW1 TASK CREATED \n");
    while(1){
         //check the button 
         if((PORT_BUTTON_SW1->IN & MASK_BUTTON_SW1)==0){
            button_count++;
            if(button_count==2)
            {
                printf("Button SW1 PRESSED--Enable Buzzer \n");
              buzzer_enable  = true;
            
            }
         
        }else{
            button_count=0;
        }
        vTaskDelay(pdMS_TO_TICKS(15));
         //optionally increment the count 

         //if falling edge decteted,set global variables 

         //delay 15ms 
    }
}
void task_button_sw2(void *arg){
    (void)arg;
    uint32_t button_count=0;
printf("SW2 task CREATED \n");
    while(1){
         //check the button 
         if((PORT_BUTTON_SW2->IN & MASK_BUTTON_SW2)==0){
            button_count++;
            if(button_count==2)
            {
                printf("SW2 PRESSED--Disable Buzzer \n");
              buzzer_enable  = false;
            }
         
        }
        else {
                button_count=0;
            }
        vTaskDelay(pdMS_TO_TICKS(15));
         //optionally increment the count 

         //if falling edge decteted,set global variables 

         //delay 15ms 
    }
}
void task_buzzer(void *arg){
    (void)arg;
    printf("buzzer task created \n");
    while(1){
        vTaskDelay(pdMS_TO_TICKS(100));
        if(buzzer_enable){
            printf("Buzzer Enabled\n\r");
            buzzer_on();

        }
        else{
            buzzer_off();
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

    /* Initialize the buttons */
buttons_init_gpio();
    /* Initialize the buzzer */
    buzzer_init(0.5,2000);
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
    xTaskCreate(
        task_button_sw1,
        "SW1 TASK",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY+1,
        NULL
    );
    xTaskCreate(
        task_button_sw2,
        "SW2 TASK",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY+1,
        NULL
    );
    xTaskCreate(
        task_buzzer,
        "Buzzer TASK",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY+1,
        NULL
    );
    
    /* Register the tasks with FreeRTOS*/

    /* Start the scheduler*/
 vTaskStartScheduler();
    /* Will never reach this loop once the scheduler starts */
    while (1)
    {
    }
}
#endif