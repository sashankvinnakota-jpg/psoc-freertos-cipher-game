/**
 * @file leds.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "leds.h"

// Map our LED enum to physical pins (order matches ece353_led_t)
static const cyhal_gpio_t LED_PINS[] = {
	P9_0, // ECE353_LED_RED
	P8_0, // ECE353_LED_BLUE
	P9_2, // ECE353_LED_GREEN
};

// Number of LEDs we support
#define NUM_LEDS (sizeof(LED_PINS) / sizeof(LED_PINS[0]))

cy_rslt_t leds_init(void)
{
	cy_rslt_t result = CY_RSLT_SUCCESS;

	for (size_t i = 0; i < NUM_LEDS; ++i)
	{
		cy_rslt_t r = cyhal_gpio_init(LED_PINS[i], CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0);
		if (r != CY_RSLT_SUCCESS)
		{
			result = r;
			break;
		}
	}

	return result;
}

void leds_set_state(ece353_led_t led, ece353_led_state_t state)
{
	if(led==ECE353_LED_RED){
		cyhal_gpio_write(LED_PINS[ECE353_LED_RED], (state == ECE353_LED_ON) ? 1 : 0);
	}
	else if(led==ECE353_LED_BLUE){
		cyhal_gpio_write(LED_PINS[ECE353_LED_BLUE], (state == ECE353_LED_ON) ? 1 : 0);
	}
	else if(led==ECE353_LED_GREEN){
		cyhal_gpio_write(LED_PINS[ECE353_LED_GREEN], (state == ECE353_LED_ON) ? 1 : 0);
	}	
	else{
		printf("Invalid LED specified\n");
	}
}
cy_rslt_t rslt;

//funtion that intializes pwm pin to intialize the io pins connected to red,green,blue leds
cy_rslt_t leds_init_pwm(
cyhal_pwm_t *pwm_obj_red,
cyhal_pwm_t *pwm_obj_green,
cyhal_pwm_t *pwm_obj_blue){


	rslt=cyhal_pwm_init(pwm_obj_red,PIN_LED_RED,NULL);
	if(rslt!=CY_RSLT_SUCCESS){
		printf("PWM RED INIT FAILED\n");
		return rslt;			
	
}
rslt=cyhal_pwm_set_duty_cycle(pwm_obj_red,0,1000);
if(rslt!=CY_RSLT_SUCCESS){
	printf("PWM RED DUTY CYCLE SET FAILED\n");	

	return rslt;
}

rslt=cyhal_pwm_start(pwm_obj_red);
if(rslt!=CY_RSLT_SUCCESS){
	printf("PWM RED START FAILED\n");

	return rslt;
}	

rslt =cyhal_pwm_init(pwm_obj_green,PIN_LED_GREEN,NULL);
if(rslt!=CY_RSLT_SUCCESS){
	printf("PWM GREEN INIT FAILED\n");
	return rslt;		

}
rslt=cyhal_pwm_set_duty_cycle(pwm_obj_green,0,1000);
if(rslt!=CY_RSLT_SUCCESS){			
	printf("PWM GREEN DUTY CYCLE SET FAILED\n");		
	return rslt;
}
rslt=cyhal_pwm_start(pwm_obj_green);
if(rslt!=CY_RSLT_SUCCESS){
	printf("PWM GREEN START FAILED\n");	
	return rslt;		
}
rslt =cyhal_pwm_init(pwm_obj_blue,PIN_LED_BLUE,NULL);
if(rslt!=CY_RSLT_SUCCESS){
	printf("PWM BLUE INIT FAILED\n");				
	return rslt;
}		
rslt=cyhal_pwm_set_duty_cycle(pwm_obj_blue,0,1000);
if(rslt!=CY_RSLT_SUCCESS){			
	printf("PWM BLUE DUTY CYCLE SET FAILED\n");		
	return rslt;				

}
rslt=cyhal_pwm_start(pwm_obj_blue);
if(rslt!=CY_RSLT_SUCCESS){
	printf("PWM BLUE START FAILED\n");	
	return rslt;	
}	
return rslt;
}	