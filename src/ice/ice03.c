/**
 * @file ice02.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE03)
#include "drivers.h"
#include <stdio.h>

char APP_DESCRIPTION[] = "ECE353: ICE 03 - Timer Interrupts/Debounce Buttons";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

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

typedef enum{
    INIT=0,
    SW1_DET,
    SW2_DET_1,
    SW2_DET_2,
    SW3_DET,
}fsm_states;
void app_init_hw(void)
{
    cy_rslt_t rslt;

    console_init();
    printf("\x1b[2J\x1b[;H");
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

    buttons_init_gpio();
    buttons_init_timer();
    leds_init();

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
    static fsm_states current_state = INIT;
    while(1)
    {
        cyhal_gpio_write(PIN_LED_RED, 0);
        cyhal_gpio_write(PIN_LED_GREEN, 0);
        cyhal_gpio_write(PIN_LED_BLUE, 0);

        switch(current_state)
        {
            case INIT:
                cyhal_gpio_write(PIN_LED_RED, 1);
                break;

            case SW1_DET:
                cyhal_gpio_write(PIN_LED_RED, 1);
                cyhal_gpio_write(PIN_LED_BLUE, 1);
                break;

            case SW2_DET_1:
                cyhal_gpio_write(PIN_LED_BLUE, 1);
                break;

            case SW2_DET_2:
                cyhal_gpio_write(PIN_LED_BLUE, 1);
                cyhal_gpio_write(PIN_LED_GREEN, 1);
                break;

            case SW3_DET:
                cyhal_gpio_write(PIN_LED_GREEN, 1);
                break;
        }

        /* ---------- FSM ---------- */
        switch(current_state)
        {
            case INIT:
                if(ECE353_Events.sw1)
                {
                    current_state = SW1_DET;
                }
                break;

            case SW1_DET:
                if(ECE353_Events.sw2)
                {
                    current_state = SW2_DET_1;
                }
                else if(ECE353_Events.sw1 || ECE353_Events.sw3)
                {
                    current_state = INIT;
                }
                break;

            case SW2_DET_1:
                if(ECE353_Events.sw2)
                {
                    current_state = SW2_DET_2;
                }
                else if(ECE353_Events.sw1 || ECE353_Events.sw3)
                {
                    current_state = INIT;
                }
                break;

            case SW2_DET_2:
                if(ECE353_Events.sw3)
                {
                    current_state = SW3_DET;
                }
                else if(ECE353_Events.sw1 || ECE353_Events.sw2)
                {
                    current_state = INIT;
                }
                break;

            case SW3_DET:
                current_state = INIT;
                break;

            default:
                printf("ICE03: Unknown State!\n");
                current_state = INIT;
                break;
        }

        /* ---------- LED Update ---------- */

        
        /* ---------- Clear Events ---------- */
        ECE353_Events.sw1 = 0;
        ECE353_Events.sw2 = 0;
        ECE353_Events.sw3 = 0;
    }
}
#endif

