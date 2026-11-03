/**
 * @file buzzer.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "buzzer.h"

/* PWM object */
static cyhal_pwm_t pwm_buzzer;

/* Store configuration */
static float pwm_duty_cycle = 0.0;
static uint32_t pwm_frequency = 0;

/**
 * @brief Initialize buzzer PWM
 * 
 * @param duty_cycle 0.0 – 1.0
 * @param frequency Frequency in Hz
 */
cy_rslt_t buzzer_init(float duty_cycle, uint32_t frequency)
{
    cy_rslt_t rslt;

    pwm_duty_cycle = duty_cycle;
    pwm_frequency = frequency;

    /* Initialize PWM on P6_2 */
    rslt = cyhal_pwm_init(&pwm_buzzer, PIN_BUZZER, NULL);
    if (rslt != CY_RSLT_SUCCESS)
        return rslt;

    /* Set duty cycle (HAL expects percent 0–100) */
    rslt = cyhal_pwm_set_duty_cycle(
                &pwm_buzzer,
                pwm_duty_cycle * 100.0,
                pwm_frequency);
    if (rslt != CY_RSLT_SUCCESS)
        return rslt;

    /* Start OFF */
    cyhal_pwm_stop(&pwm_buzzer);

    return CY_RSLT_SUCCESS;
}

/**
 * @brief Turn buzzer ON
 */
void buzzer_on(void)
{
    cyhal_pwm_start(&pwm_buzzer);
}

/**
 * @brief Turn buzzer OFF
 */
void buzzer_off(void)
{
    cyhal_pwm_stop(&pwm_buzzer);
}