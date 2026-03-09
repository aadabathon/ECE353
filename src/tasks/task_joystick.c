/**
 * @file task_joystick.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief FreeRTOS task for monitoring joystick position changes.
 *
 * This task periodically polls the joystick, compares the current position
 * against the previous position, and only reports changes. When a change is
 * detected, the latest joystick position is written to a depth-1 queue and
 * EVENT_JOYSTICK is set in the shared RTOS event group.
 *
 * The queue is intentionally length 1 because only the most recent joystick
 * position matters for this application.
 *
 * @version 0.1
 * @date 2025-08-14
 *
 * @copyright Copyright (c) 2025
 */

#include "main.h"

#ifdef ECE353_FREERTOS

#include "drivers.h"
#include "task_joystick.h"

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/* Queue that stores the most recent joystick position */
QueueHandle_t Queue_Joystick = NULL;

/* Shared RTOS event group used by the application tasks */
extern EventGroupHandle_t ECE353_RTOS_Events;

/*
 * Human-readable strings for debug printing.
 * The order must match the joystick_position_t enum values.
 */
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

/*****************************************************************************/
/* Task Definitions                                                          */
/*****************************************************************************/

/**
 * @brief Task that monitors the joystick.
 *
 * This task polls the joystick every 25 ms. If the joystick position has
 * changed since the previous sample, the new position is written to the
 * joystick queue and EVENT_JOYSTICK is set in the event group so other
 * tasks can respond.
 *
 * Only position changes are reported, which avoids repeatedly generating
 * events while the joystick remains held in the same direction.
 *
 * @param arg
 * Unused task parameter.
 */
void task_joystick(void *arg)
{
    (void)arg;

    /* Start assuming the joystick is centered */
    joystick_position_t prev = JOYSTICK_POS_CENTER;

    /* Used to maintain a fixed polling period */
    TickType_t last_wake = xTaskGetTickCount();

    for (;;)
    {
        joystick_position_t cur = joystick_get_pos();

        /* Only notify the system when the joystick position changes */
        if (cur != prev)
        {
            /*
             * The queue length is 1 by design, so overwrite is appropriate:
             * the consumer only needs the most recent joystick position.
             */
            xQueueOverwrite(Queue_Joystick, &cur);

            /* Signal that a new joystick event is available */
            xEventGroupSetBits(ECE353_RTOS_Events, EVENT_JOYSTICK);

            /* Debug print showing the decoded joystick position */
            printf("Joystick Position: %s\r\n", joystick_pos_names[cur]);

            prev = cur;
        }

        /* Poll the joystick at a fixed 25 ms rate */
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(25));
    }
}

/*****************************************************************************/
/* Initialization                                                            */
/*****************************************************************************/

/**
 * @brief Create the joystick queue and joystick task.
 *
 * The joystick queue is length 1 because the application only needs the
 * latest position sample, not a history of all movements.
 *
 * @return true if initialization succeeded
 * @return false if queue or task creation failed
 */
bool task_joystick_init(void)
{
    /* Create a depth-1 queue to hold the latest joystick position */
    Queue_Joystick = xQueueCreate(1, sizeof(joystick_position_t));
    if (Queue_Joystick == NULL)
    {
        return false;
    }

    /* Create the joystick polling task */
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