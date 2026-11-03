#include "buttons.h"
#include "cyhal_gpio.h"
#include "ece353-events.h"
#include "ece353-pins.h"




#define REG_IN_SW (*(volatile uint32_t *)(0x40310310))

/* Track previous states internally */
static bool prev_sw1 = true;
static bool prev_sw2 = true;
static bool prev_sw3 = true;

cy_rslt_t buttons_init_gpio(void)
{
    cyhal_gpio_init(PIN_BUTTON_SW1, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, true);
    cyhal_gpio_init(PIN_BUTTON_SW2, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, true);
    cyhal_gpio_init(PIN_BUTTON_SW3, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, true);
    return CY_RSLT_SUCCESS;
}

button_state_t button_get_state(ece353_button_t button)
{
    bool current_state;
     static bool prev_state_SW1;
        static bool prev_state_SW2;
        static bool prev_state_SW3;
        bool *prev_state;

    switch(button)
    {
        case PIN_BUTTON_SW1:
            current_state = (REG_IN_SW & MASK_BUTTON_SW1) ? true : false;
            prev_state= &prev_state_SW1;
            prev_state_SW1= current_state;
            break;

        case PIN_BUTTON_SW2:
            current_state = (REG_IN_SW & MASK_BUTTON_SW2) ? true : false;
           prev_state= &prev_state_SW2;
            prev_state_SW2= current_state;
            break;

        case PIN_BUTTON_SW3:
            current_state = (REG_IN_SW & MASK_BUTTON_SW3) ? true : false;
           prev_state= &prev_state_SW3;
            prev_state_SW3= current_state;
            break;

        default:
            return BUTTON_STATE_LOW;
    }

    button_state_t state;

    if (*prev_state == true && current_state == false)
        state = BUTTON_STATE_FALLING_EDGE;
    else if (*prev_state == false && current_state == true)
        state = BUTTON_STATE_RISING_EDGE;
    else if (current_state == false)
        state = BUTTON_STATE_LOW;
    else
        state = BUTTON_STATE_HIGH;

    *prev_state = current_state;
    return state;
}
static cyhal_timer_t button_timer;
static cyhal_timer_cfg_t button_timer_cfg;
static void button_timer_handler(void *arg, cyhal_timer_event_t event)
{
    static uint8_t button_counts[3]={[0]=0,[1]=0,[2]=0};
    uint8_t sw1=PORT_BUTTON_SW1->IN & MASK_BUTTON_SW1;
    uint8_t sw2=PORT_BUTTON_SW2->IN & MASK_BUTTON_SW2;
    uint8_t sw3=PORT_BUTTON_SW3->IN & MASK_BUTTON_SW3;    if(sw1==0){
        button_counts[0]++;
        if(button_counts[0]==5){
            ECE353_Events.sw1=1;
        }
    }
    else{
        button_counts[0]=0;
    }
    if(sw2==0){
        button_counts[1]++;
        if(button_counts[1]==5){
            ECE353_Events.sw2=1;
        }
    }
    else{
        button_counts[1]=0;
    }
    if(sw3==0){
        button_counts[2]++;
        if(button_counts[2]==5){
            ECE353_Events.sw3=1;
        }
    }
    else{
        button_counts[2]=0;
    }


    // Timer interrupt handler for button debouncing
    // This function can be expanded to handle debouncing logic if needed
}
// Function to initialize the timer for button debouncing
cy_rslt_t buttons_init_timer(void){
    return timer_init(&button_timer, &button_timer_cfg, 500000, button_timer_handler);

}
 