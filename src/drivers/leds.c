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
                    CYHAL_GPIO_DRIVE_STRONG, 0);

    cyhal_gpio_init(PIN_LED_GREEN, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, 0);

    cyhal_gpio_init(PIN_LED_BLUE,  CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, 0);
                    
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

cy_rslt_t leds_pwm_init( 
    cyhal_pwm_t *pwm_obj_red,
    cyhal_pwm_t *pwm_obj_green,
    cyhal_pwm_t *pwm_obj_blue
)
{
    cy_rslt_t rslt;

    // Initialize PWM for Red LED
    rslt = cyhal_pwm_init(pwm_obj_red, PIN_LED_RED, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        return rslt;
    }

    // Initialize PWM for Green LED
    rslt = cyhal_pwm_init(pwm_obj_green, PIN_LED_GREEN, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        return rslt;
    }

    // Initialize PWM for Blue LED
    rslt = cyhal_pwm_init(pwm_obj_blue, PIN_LED_BLUE, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        return rslt;
    }

    return CY_RSLT_SUCCESS;
}
