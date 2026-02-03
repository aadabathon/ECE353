/**
 * @file ice02.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE03)
#include "drivers.h"
#include <stdio.h>

char APP_DESCRIPTION[] = "ECE353: ICE 03 - Timer Interrupts/Debounce Buttons";

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

}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
typedef enum {
    STATE_INIT,
    STATE_SW1_DET,
    STATE_SW2_DET_1,
    STATE_SW2_DET_2,
    STATE_SW3_DET,
} ice03_state_t;

void main_app(void)
{
    ice03_state_t current_state = STATE_INIT;

    while (1)
    {
        if (ECE353_Events.sw1 || ECE353_Events.sw2 || ECE353_Events.sw3)
        {
            // snapshot events then clear them (so one press = one transition)
            bool sw1 = ECE353_Events.sw1;
            bool sw2 = ECE353_Events.sw2;
            bool sw3 = ECE353_Events.sw3;

            ECE353_Events.sw1 = 0;
            ECE353_Events.sw2 = 0;
            ECE353_Events.sw3 = 0;

            switch (current_state)
            {
                case STATE_INIT:
                    if (sw1) current_state = STATE_SW1_DET;      // SW1 advances
                    // SW2|SW3 => stay in INIT (self-loop)
                    break;

                case STATE_SW1_DET:
                    if (sw2) current_state = STATE_SW2_DET_1;    // SW2 advances
                    else if (sw1 || sw3) current_state = STATE_INIT; // SW1|SW3 reset
                    break;

                case STATE_SW2_DET_1:
                    if (sw2) current_state = STATE_SW2_DET_2;    // SW2 advances
                    else if (sw1 || sw3) current_state = STATE_INIT; // SW1|SW3 reset
                    break;

                case STATE_SW2_DET_2:
                    if (sw3) current_state = STATE_SW3_DET;      // SW3 advances
                    else if (sw1 || sw2) current_state = STATE_INIT; // SW1|SW2 reset
                    break;

                case STATE_SW3_DET:
                    if (sw1 || sw2 || sw3) current_state = STATE_INIT; // any button resets
                    break;

                default:
                    current_state = STATE_INIT;
                    break;
            }

            // 3) LED outputs per state (per your labels)
            switch (current_state)
            {
                case STATE_INIT:      /* Red */         set_rgb(1,0,0); break;
                case STATE_SW1_DET:   /* Red+Blue */    set_rgb(1,0,1); break;
                case STATE_SW2_DET_1: /* Blue */        set_rgb(0,0,1); break;
                case STATE_SW2_DET_2: /* Blue+Green */  set_rgb(0,1,1); break;
                case STATE_SW3_DET:   /* Green */       set_rgb(0,1,0); break;
                default: break;
            }
        }
    }
}
#endif