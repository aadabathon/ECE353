/**
 * @file task_console_rx.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "task_console.h"

#ifdef ECE353_FREERTOS  
/**
 * @brief 
 * This function is the event handler for the console UART.
 *
 * The ISR will receive characters from the UART and store them in a console buffer
 * until the user presses the ENTER key.  At that point, the ISR will send a task
 * notification to the console task to process the received string.
 *
 * The ISR will also echo the received character back to the console.
 *
 * @param handler_arg 
 * @param event 
 */
void console_event_handler(void *handler_arg, cyhal_uart_event_t event)
{
    (void)handler_arg;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t c;

    if ((event & CYHAL_UART_IRQ_RX_NOT_EMPTY) == 0)
    {
        return;
    }

    // Drain all available RX chars
    while (cyhal_uart_getc(&cy_retarget_io_uart_obj, &c, 0) == CY_RSLT_SUCCESS)
    {

        // Echo back
        cyhal_uart_putc(&cy_retarget_io_uart_obj, c);

        // Backspace / Delete
        if ((c == '\b') || (c == 127))
        {
            if (produce_console_buffer->index > 0)
            {
                produce_console_buffer->index--;
                produce_console_buffer->data[produce_console_buffer->index] = '\0';
            }
        }
        // Enter
        else if ((c == '\r') || (c == '\n'))
        {
            if (produce_console_buffer->index > 0)
            {
                produce_console_buffer->data[produce_console_buffer->index] = '\0';

                volatile console_buffer_t *temp = consume_console_buffer;
                consume_console_buffer = produce_console_buffer;
                produce_console_buffer = temp;

                produce_console_buffer->index = 0;
                produce_console_buffer->data[0] = '\0';
                
                cyhal_gpio_toggle(PIN_LED_RED);
                vTaskNotifyGiveFromISR(TaskHandle_Console_Rx, &xHigherPriorityTaskWoken);
            }
        }
        // Normal char
        else
        {
            if (produce_console_buffer->index < (CONSOLE_MAX_MESSAGE_LENGTH - 1))
            {
                produce_console_buffer->data[produce_console_buffer->index++] = (char)c;
                produce_console_buffer->data[produce_console_buffer->index] = '\0';
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/**
 * @brief
 * This function initializes the console tasks and resources.
 * @return true
 * @return false
 */
bool task_console_init(void)
{
    /* Register a function for the UART ISR*/
    cyhal_uart_register_callback(
        &cy_retarget_io_uart_obj,           // UART object
        console_event_handler,         // Event handler
        NULL                       // Handler argument
    );

    /* Initialize UART Rx Resources */
    if (!task_console_resources_init_rx())
    {
        return false; // Initialization failed
    }

    /* Initialize UART Tx Resources */
    if (!task_console_resources_init_tx())
    {
        return false; // Initialization failed
    }
    else
    {
        // Enable UART Rx Interrupts
        cyhal_uart_enable_event(
            &cy_retarget_io_uart_obj, 
            CYHAL_UART_IRQ_RX_NOT_EMPTY, 
            7, 
            true);
    }
    
    return true; // Initialization successful
}
#endif  