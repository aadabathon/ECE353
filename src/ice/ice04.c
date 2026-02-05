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

char APP_DESCRIPTION[] = "ECE353: ICE 04 - PWM Buzzer";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
cyhal_pwm_t pwm_red;
cyhal_pwm_t pwm_green;
cyhal_pwm_t pwm_blue;
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
    
    leds_pwm_init(&pwm_red, &pwm_green, &pwm_blue);

    

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
    int red = 0;
    int green = 0;
    int blue = 0;


    while(1)
    {
        /* ADD CODE */
        button_state_t sw1_state = buttons_get_state(BUTTON_SW1);
        if (sw1_state == BUTTON_STATE_FALLING_EDGE)
        {
            red = (red + 10);
            if (red >= 100) {
                red = 0;
            }
            printf("Red Intensity: %d\n", red);
            cyhal_pwm_set_duty_cycle(&pwm_red, red, 100.0);
        }
        button_state_t sw2_state = buttons_get_state(BUTTON_SW2);
        if (sw2_state == BUTTON_STATE_FALLING_EDGE)
        {
            green = (green + 10);
            if (green >= 100) {
                green = 0;
            }   
            printf("Green Intensity: %d\n", green);
            cyhal_pwm_set_duty_cycle(&pwm_green, green, 100.0);
        }
        button_state_t sw3_state = buttons_get_state(BUTTON_SW3);
        if (sw3_state == BUTTON_STATE_FALLING_EDGE)
        {
            blue = (blue + 10); 
            if (blue >= 100) {
                blue = 0;
            }
            printf("Blue Intensity: %d\n", blue);
            cyhal_pwm_set_duty_cycle(&pwm_blue, blue, 100.0);
        }

        /* END ADD CODE */
    }
}
#endif
