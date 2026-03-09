/**
 * @file task_buttons.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief FreeRTOS task for debouncing push buttons.
 *
 * This task periodically samples the button GPIO inputs and performs
 * software debouncing. When a stable falling edge is detected on a
 * button input, the corresponding event bit is set in the shared RTOS
 * event group.
 *
 * Current HW02 usage only requires SW1, so SW2 and SW3 are left out
 * of the active debounce loop for now.
 *
 * @version 0.1
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025
 */
#include "task_console.h"
#include "task_buttons.h"

#ifdef ECE353_FREERTOS

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/* Handle for the button task */
TaskHandle_t TaskHandle_Buttons = NULL;

/* Shared application event group */
extern EventGroupHandle_t ECE353_RTOS_Events;

/*****************************************************************************/
/* Helper Functions                                                          */
/*****************************************************************************/

/**
 * @brief Update the debounce state for one button input.
 *
 * This function tracks:
 * - the most recent raw sample
 * - how many consecutive times that raw value has been observed
 * - the current debounced stable state
 *
 * If the raw input remains unchanged for @p threshold consecutive samples
 * and that value differs from the stored stable state, the stable state is
 * updated. A button press is reported only on a stable falling edge
 * (active-low transition from 1 to 0).
 *
 * @param d
 * Pointer to the debounce state structure for this button.
 *
 * @param raw
 * Current raw GPIO sample for the button.
 *
 * @param threshold
 * Number of consecutive identical samples required before updating the
 * debounced stable state.
 *
 * @param pressed_edge
 * Output flag set to true only when a stable active-low press edge is
 * detected during this update.
 */
void debounce_update(debounce_t *d, uint8_t raw, uint8_t threshold, bool *pressed_edge)
{
    *pressed_edge = false;

    /* Count consecutive identical raw samples */
    if (raw == d->last_raw)
    {
        if (d->cnt < threshold)
        {
            d->cnt++;
        }
    }
    else
    {
        d->last_raw = raw;
        d->cnt = 1;
    }

    /*
     * Once the input has remained unchanged long enough, update the
     * stable value. Report a press only on a stable falling edge,
     * since the buttons are active-low.
     */
    if ((d->cnt >= threshold) && (d->stable != raw))
    {
        uint8_t prev = d->stable;
        d->stable = raw;

        if ((prev == 1) && (raw == 0))
        {
            *pressed_edge = true;
        }
    }
}

/*****************************************************************************/
/* Task Definitions                                                          */
/*****************************************************************************/

/**
 * @brief Task that debounces the push buttons.
 *
 * The task samples each enabled button every 15 ms. A button press is
 * recognized after 2 consecutive identical samples, corresponding to
 * a 30 ms debounce interval.
 *
 * For HW02, only SW1 is used. When SW1 is pressed, EVENT_SW1 is set in
 * the shared RTOS event group so the system control task can respond.
 *
 * @param arg
 * Unused task parameter.
 */
void task_buttons(void *arg)
{
    (void)arg;

    /* 2 consecutive 15 ms samples = 30 ms debounce window */
    const uint8_t TH = 2;

    /* Debounce state for SW1 (active-low button, idle state = 1) */
    debounce_t sw1 = { .stable = 1, .last_raw = 1, .cnt = 0 };

    /* Optional future debounce states for SW2/SW3 */
    // debounce_t sw2 = { .stable = 1, .last_raw = 1, .cnt = 0 };
    // debounce_t sw3 = { .stable = 1, .last_raw = 1, .cnt = 0 };

    while (1)
    {
        bool edge_sw1;

        /* Sample SW1 and update its debounce state */
        debounce_update(&sw1, cyhal_gpio_read(PIN_BUTTON_SW1), TH, &edge_sw1);

        /* Signal a button event only on a stable falling edge */
        if (edge_sw1)
        {
            xEventGroupSetBits(ECE353_RTOS_Events, EVENT_SW1);
            printf("SW1 Pressed!\r\n");
        }

        /* Sample buttons every 15 ms */
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

/*****************************************************************************/
/* Initialization                                                            */
/*****************************************************************************/

/**
 * @brief Initialize button GPIO hardware and create the button task.
 *
 * @return true if the task was created successfully
 * @return false if task creation failed
 */
bool task_buttons_init(void)
{
    BaseType_t result;

    /* Initialize button GPIO pins */
    buttons_init_gpio();

    /* Create the FreeRTOS button debounce task */
    result = xTaskCreate(
        task_buttons,
        "Button Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &TaskHandle_Buttons
    );

    if (result != pdPASS)
    {
        return false;
    }

    return true;
}

#endif