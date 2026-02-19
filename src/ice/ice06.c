/**
 * @file ex03.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE06)
#include "drivers.h"
#include "task_joystick.h"

char APP_DESCRIPTION[] = "ECE353: ICE 06 - FreeRTOS Queues";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
/* Create a lookup table to print out the joystick positions*/
static const char * const joystick_pos_names[] = {
  "Center","Left","Right","Up","Down","Upper Left","Upper Right","Lower Left","Lower Right"
};


const char *Joystick_Pos_Strings[] = {
    [JOYSTICK_POS_CENTER] = "Center",
    [JOYSTICK_POS_LEFT] = "Left",
    [JOYSTICK_POS_RIGHT] = "Right",
    [JOYSTICK_POS_UP] = "Up",
    [JOYSTICK_POS_DOWN] = "Down",
    [JOYSTICK_POS_UPPER_LEFT] = "Upper Left",
    [JOYSTICK_POS_UPPER_RIGHT] = "Upper Right",
    [JOYSTICK_POS_LOWER_LEFT] = "Lower Left",
    [JOYSTICK_POS_LOWER_RIGHT] = "Lower Right"
};

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
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

}

void task_print_directions(void *arg)
{
    (void)arg;

    for (;;)
    {
        joystick_position_t pos;

        if (xQueueReceive(Queue_Joystick, &pos, portMAX_DELAY) == pdPASS)
        {
            if ((uint32_t)pos < (sizeof(joystick_pos_names)/sizeof(joystick_pos_names[0])))
            {
                printf("Joystick Position Changed: %s\n\r", joystick_pos_names[pos]);
            }
            else
            {
                printf("Joystick Position Changed: <invalid>\n\r");
            }
        }
    }
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
    /* Initialize joystick resources */
    joystick_init();

    /* Register the tasks with FreeRTOS */
    if (!task_joystick_init())
    {
        while (1) {} // failed to create queue/task
    }

    xTaskCreate(
        task_print_directions,
        "PrintJoy",
        configMINIMAL_STACK_SIZE + 256,
        NULL,
        1,
        NULL
    );

    /* Start the scheduler */
    vTaskStartScheduler();

    while (1) {}
}

#endif