/**
 * @file buttons.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "buttons.h"


 // gpio intitialization is usually as follows:
 // cyhal_gpio_init(pin, direction, drive_mode, initial_value);


cy_rslt_t buttons_init_gpio(void)
{
    cyhal_gpio_init(PIN_BUTTON_SW1, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, 1);
    cyhal_gpio_init(PIN_BUTTON_SW2, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, 1);
    cyhal_gpio_init(PIN_BUTTON_SW3, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, 1);

    return CY_RSLT_SUCCESS;
}

button_state_t buttons_get_state(ece353_button_t button)
{
    static uint8_t prev_state[3] = {1, 1, 1};
    uint8_t curr_state = 1;

    switch (button)
    {
        case BUTTON_SW1:
            curr_state = (PORT_BUTTON_SW1->IN & MASK_BUTTON_PIN_SW1) ? 1 : 0;
            break;
        case BUTTON_SW2:
            curr_state = (PORT_BUTTON_SW2->IN & MASK_BUTTON_PIN_SW2) ? 1 : 0;
            break;
        case BUTTON_SW3:
            curr_state = (PORT_BUTTON_SW3->IN & MASK_BUTTON_PIN_SW3) ? 1 : 0;
            break;
    }

    button_state_t result;

    if (prev_state[button] == 1 && curr_state == 0)
        result = BUTTON_STATE_FALLING_EDGE;
    else if (prev_state[button] == 0 && curr_state == 1)
        result = BUTTON_STATE_RISING_EDGE;
    else if (curr_state == 1)
        result = BUTTON_STATE_HIGH;
    else
        result = BUTTON_STATE_LOW;

    prev_state[button] = curr_state;
    return result;
}