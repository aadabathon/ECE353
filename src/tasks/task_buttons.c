/**
 * @file task_buttons.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "task_console.h"
#include "task_buttons.h"

 #ifdef ECE353_FREERTOS
 /**
  * @brief 
  * Task used to debounce button presses (SW1, SW2, SW3).  
  * The falling edge of the button press is detected by de-bouncing
  * the button for 30mS. Each button should be sampled every 15mS.
  *
  * When a button press is detected, the corresponding event is set in
  * in the event group ECE353_RTOS_Events.
  *
  * @param arg 
  * Unused parameter
  */

TaskHandle_t TaskHandle_Buttons = NULL;
extern EventGroupHandle_t ECE353_RTOS_Events;
  
void debounce_update(debounce_t *d, uint8_t raw, uint8_t threshold, bool *pressed_edge)
{
    *pressed_edge = false;

    if (raw == d->last_raw) {
        if (d->cnt < threshold) d->cnt++;
    } else {
        d->last_raw = raw;
        d->cnt = 1;
    }

    if (d->cnt >= threshold && d->stable != raw) {
        uint8_t prev = d->stable;
        d->stable = raw;
        if (prev == 1 && raw == 0) *pressed_edge = true;  // active-low edge
    }
}

void task_buttons(void *arg)
{
    (void)arg;

    const uint8_t TH = 2; // 2 consecutive samples = 30 ms (since we sample every 15 ms)

    debounce_t sw1 = { .stable = 1, .last_raw = 1, .cnt = 0 };
    //debounce_t sw2 = { .stable = 1, .last_raw = 1, .cnt = 0 };
    //debounce_t sw3 = { .stable = 1, .last_raw = 1, .cnt = 0 };

    while (1)
    {
        bool edge_sw1;

        debounce_update(&sw1, cyhal_gpio_read(PIN_BUTTON_SW1), TH, &edge_sw1);

        if (edge_sw1) {
            xEventGroupSetBits(ECE353_RTOS_Events, EVENT_SW1);
            printf("SW1 Pressed!\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

 /* Button Task Initialization */
bool task_buttons_init(void){

    BaseType_t result;

    buttons_init_gpio();

    // Create the button task
    result = xTaskCreate(
        task_buttons, 
        "Button Task", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        &TaskHandle_Buttons
    );

    if(result != pdPASS)
    {
        return false;
    }

    return true;
}
#endif