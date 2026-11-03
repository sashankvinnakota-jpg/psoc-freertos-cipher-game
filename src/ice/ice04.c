/**
 * @file ice04.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE04)
#include "drivers.h"
#include <stdio.h>
    cyhal_pwm_t pwm_obj_red;
    cyhal_pwm_t pwm_obj_green;
    cyhal_pwm_t pwm_obj_blue;

char APP_DESCRIPTION[] = "ECE353: ICE 04 - PWM Buzzer";

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
    leds_init_pwm(&pwm_obj_red,&pwm_obj_green,&pwm_obj_blue );
    /* ADD CODE */

}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void app_main(void)
{ static uint8_t red_intensity = 0;
    static uint8_t green_intensity = 0;
    static uint8_t blue_intensity = 0;
    while(1)
    { if (cyhal_gpio_read(PIN_BUTTON_SW1) == 0)
        {
            red_intensity += 10;
            if (red_intensity > 100)
            {
                red_intensity = 0;
            }

            cyhal_pwm_set_duty_cycle(&pwm_obj_red, red_intensity, 1000);
            printf("Red LED Intensity: %d%%\n\r", red_intensity);

            cyhal_system_delay_ms(200); // debounce
        }

        if (cyhal_gpio_read(PIN_BUTTON_SW2) == 0)
        {
            green_intensity += 10;
            if (green_intensity > 100)
            {
                green_intensity = 0;
            }

            cyhal_pwm_set_duty_cycle(&pwm_obj_green, green_intensity, 1000);
            printf("Green LED Intensity: %d%%\n\r", green_intensity);

            cyhal_system_delay_ms(200);
        }

        if (cyhal_gpio_read(PIN_BUTTON_SW3) == 0)
        {
            blue_intensity += 10;
            if (blue_intensity > 100)
            {
                blue_intensity = 0;
            }

            cyhal_pwm_set_duty_cycle(&pwm_obj_blue, blue_intensity, 1000);
            printf("Blue LED Intensity: %d%%\n\r", blue_intensity);

            cyhal_system_delay_ms(200);
        }
    }
}
#endif
