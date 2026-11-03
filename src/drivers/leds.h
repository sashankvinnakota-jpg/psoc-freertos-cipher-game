/**
 * @file leds.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __LEDS_H__
#define __LEDS_H__
#include "cyhal_pwm.h"

#include <stdio.h>
#include "cybsp.h"
#include "cyhal_gpio.h"
#include "ece353-pins.h"

// Enumerated type identifying LEDs on the board
typedef enum ece353_led
{
	ECE353_LED_RED = 0,
	ECE353_LED_BLUE,
	ECE353_LED_GREEN,
} ece353_led_t;

// Enumerated type for LED state
typedef enum ece353_led_state
{
	ECE353_LED_OFF = 0,
	ECE353_LED_ON  = 1,
} ece353_led_state_t;

// Initialize LEDs (configure GPIOs/pins)
cy_rslt_t leds_init(void);

// Set the state of a specific LED
void leds_set_state(ece353_led_t led, ece353_led_state_t state);
   // Function that configures the RGB LED pins to be controlled by PWM
cy_rslt_t leds_init_pwm(
cyhal_pwm_t *pwm_obj_red, 
cyhal_pwm_t *pwm_obj_green, 
cyhal_pwm_t *pwm_obj_blue
);


#endif