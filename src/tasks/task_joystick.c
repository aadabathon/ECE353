/**
 * @file task_joystick.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "main.h"

#ifdef ECE353_FREERTOS  
#include "drivers.h"
 #include "task_joystick.h"

QueueHandle_t Queue_Joystick = NULL;

/* Message lookup table for joystick positions */
const char * const joystick_pos_names[] = {
    "Center",
    "Left",
    "Right",
    "Up",
    "Down",
    "Upper Left",
    "Upper Right",
    "Lower Left",
    "Lower Right"
};

 /**
  * @brief 
  *  Task used to monitor the joystick
  * @param arg 
  */
void task_joystick(void *arg)
{
    (void)arg;

    joystick_position_t prev = JOYSTICK_POS_CENTER;   
    TickType_t last_wake = xTaskGetTickCount();

    for (;;)
    {
        joystick_position_t cur = joystick_get_pos();

        if (cur != prev)
        {
            // Queue length is 1 → overwrite is perfect 
            xQueueOverwrite(Queue_Joystick, &cur);
            xEventGroupSetBits(ECE353_RTOS_Events, EVENT_JOYSTICK);
            prev = cur;
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(25)); // Poll every 25 ms
    }
}

bool task_joystick_init(void)
{
    Queue_Joystick = xQueueCreate(1, sizeof(joystick_position_t));
    if (Queue_Joystick == NULL) return false;

    BaseType_t ok = xTaskCreate(
        task_joystick,
        "Joystick",
        configMINIMAL_STACK_SIZE + 256,
        NULL,
        2,
        NULL
    );

    return (ok == pdPASS);
}
#endif