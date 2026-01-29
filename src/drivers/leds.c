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

 cy_rslt_t leds_init_gpio(void)
{
    cyhal_gpio_init(PIN_LED_RED,   CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, 1);

    cyhal_gpio_init(PIN_LED_GREEN, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, 1);

    cyhal_gpio_init(PIN_LED_BLUE,  CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, 1);
                    
    return CY_RSLT_SUCCESS;
}

void leds_set_state(ece353_led_t led, ece353_led_state_t state)
{
    GPIO_PRT_Type *port;
    uint32_t mask;

    switch (led)
    {
        case LED_RED:
            port = PORT_LED_RED;
            mask = MASK_LED_RED;
            break;

        case LED_GREEN:
            port = PORT_LED_GREEN;
            mask = MASK_LED_GREEN;
            break;

        case LED_BLUE:
            port = PORT_LED_BLUE;
            mask = MASK_LED_BLUE;
            break;

        default:
            return;
    }

    if (state == LED_ON)
    {
        port->OUT_CLR = mask;   // active-low → ON
    }
    else
    {
        port->OUT_SET = mask;   // OFF
    }
}

