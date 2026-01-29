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
    buttons_init_gpio();
    leds_init_gpio();
    console_init();
    printf("\x1b[2J\x1b[;H");
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

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
    while (1)
    {
        button_state_t sw1 = buttons_get_state(BUTTON_SW1);
        button_state_t sw2 = buttons_get_state(BUTTON_SW2);
        button_state_t sw3 = buttons_get_state(BUTTON_SW3);

        if (sw1 == BUTTON_STATE_FALLING_EDGE)
        {
            printf("SW1 pressed\n");
            leds_set_state(LED_RED, LED_ON);
        }
        else if (sw1 == BUTTON_STATE_RISING_EDGE)
        {
            printf("SW1 released\n");
            leds_set_state(LED_RED, LED_OFF);
        }

        if (sw2 == BUTTON_STATE_FALLING_EDGE)
        {
            printf("SW2 pressed\n");
            leds_set_state(LED_GREEN, LED_ON);
        }
        else if (sw2 == BUTTON_STATE_RISING_EDGE)
        {
            printf("SW2 released\n");
            leds_set_state(LED_GREEN, LED_OFF);
        }

        if (sw3 == BUTTON_STATE_FALLING_EDGE)
        {
            printf("SW3 pressed\n");
            leds_set_state(LED_BLUE, LED_ON);
        }
        else if (sw3 == BUTTON_STATE_RISING_EDGE)
        {
            printf("SW3 released\n");
            leds_set_state(LED_BLUE, LED_OFF);
        }

        cyhal_system_delay_ms(50);
    }
}

#endif
