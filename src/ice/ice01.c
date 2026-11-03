/**
 * @file ice01.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE01)
#include "drivers.h"

char APP_DESCRIPTION[] = "ECE353: ICE 01 - Memory Mapped IO - GPIO";

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
void app_init_hw(void)
{
    console_init();
    printf("\x1b[2J\x1b[;H");
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");
buttons_init_gpio();
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
button_state_t state;
while(1)
    {
        state = button_get_state(BUTTON_SW1);
        if( state == BUTTON_STATE_FALLING_EDGE){
            printf("SW1 Pressed\n\r");
            leds_set_state(ECE353_LED_RED, ECE353_LED_ON);
        }
        else if(state == BUTTON_STATE_RISING_EDGE){
            printf("SW1 Released\n\r");
            leds_set_state(ECE353_LED_RED, ECE353_LED_OFF);
        }

        state = button_get_state(BUTTON_SW2);
        if( state == BUTTON_STATE_FALLING_EDGE){
            printf("SW2 Pressed\n\r");
            leds_set_state(ECE353_LED_GREEN, ECE353_LED_ON);
        }
        else if(state == BUTTON_STATE_RISING_EDGE){
            printf("SW2 Released\n\r");
            leds_set_state(ECE353_LED_GREEN, ECE353_LED_OFF);
        }

        state = button_get_state(BUTTON_SW3);
        if( state == BUTTON_STATE_FALLING_EDGE){
            printf("SW3 Pressed\n\r");
            leds_set_state(ECE353_LED_BLUE, ECE353_LED_ON);
        }
        else if(state == BUTTON_STATE_RISING_EDGE){
            printf("SW3 Released\n\r");
            leds_set_state(ECE353_LED_BLUE, ECE353_LED_OFF);
        }

        /* Sleep for 50mS */
        cyhal_system_delay_ms(100);

    }

}


#endif
