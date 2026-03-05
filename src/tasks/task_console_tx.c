/**
 * @file task_console.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */
 #include "main.h"

#ifdef ECE353_FREERTOS
#include "drivers.h"
#include "task_console.h"
#include "cyhal_uart.h"
#include "circular_buffer.h"

/**
 * @brief
 * This file contains the implementation of the console transmit (Tx) task.
 * The task is responsible for sending characters to the UART.
 * 
 * Tasks can print messages by sending the string to task_console_tx() using
 * a FreeRTOS queue.
 *
 * task_console_tx() will add the characters to a circular buffer that is
 * accessed by the UART interrupt service routine (ISR).
 *
 */

/* ADD CODE*/
/* Global Variables */

QueueHandle_t console_tx_queue = NULL; // FreeRTOS queue handle for console Tx
TaskHandle_t TaskHandle_Console_Tx = NULL;
circular_buffer_t* console_tx_cb = NULL;

/**
 * @brief 
 * This task is used to transmit characters to the UART
 * @param param 
 */
void task_console_tx(void *param)
{
    (void)param; // Unused parameter
    console_buffer_t tx_msg;

    while (1)
    {
        //WAIT for console buffer messages from the queue
        xQueueReceive(console_tx_queue, &tx_msg, portMAX_DELAY);
        //For loop examines each message and adds each byte into the circular buffer
        for (int i = 0; i < tx_msg.index; i++) 
            {
                    // If the circular buffer is full, wait until there is space
                    while (circular_buffer_get_num_bytes(console_tx_cb) >= console_tx_cb->max_size) 
                    {
                        vTaskDelay(pdMS_TO_TICKS(5)); // Delay to prevent busy waiting
                    }
            taskENTER_CRITICAL();
            circular_buffer_add(console_tx_cb, tx_msg.data[i]);
            taskEXIT_CRITICAL();
            
            cyhal_uart_enable_event(&cy_retarget_io_uart_obj, CYHAL_UART_IRQ_TX_EMPTY, CYHAL_ISR_PRIORITY_DEFAULT, true);

        }
        vPortFree(tx_msg.data);
            // once not full, add next byte, to the CB

            //enable the transmit empty interrupts, so the ISR can send the data from the CB to the UART

        //Free the data that was sent frm the CB
    }
}

/**
 * @brief 
 * This function initializes the resources for the console Tx task. 
 * @return true  if initialization is successful
 * @return false if initialization fails
 * @return false 
 */
bool task_console_resources_init_tx(void)
{
    BaseType_t rslt = pdPASS;

    //Init the console Tx FreeRTOS
    console_tx_queue = xQueueCreate(10, sizeof(console_buffer_t));
    if (console_tx_queue == NULL)
    {
        return false; // Queue creation failed
    }
    //init the circular buffer for the console Tx
    console_tx_cb = circular_buffer_init(128);
    if (console_tx_cb == NULL)
    {
        return false; // Circular buffer initialization failed
    }
    //create the freertos task
    rslt = xTaskCreate(task_console_tx,
                       "Console Tx",
                       configMINIMAL_STACK_SIZE,
                       NULL,
                       7,
                       &TaskHandle_Console_Tx);
    
    if (rslt != pdPASS)
    {
        return false; // Initialization failed
    }

    return true; // Resources initialized successfully
}

/**
 * @brief
 * This function sends formatted messages to task_console_tx. It acts as a wrapper around the FreeRTOS queue
 * to send messages so other tasks can use it easily.
 *
 * Example usage: 
 * task_console_printf("Send Message");
 * task_console_printf("Formatted number: %d", 42);
 *
 * @param str_ptr Pointer to the format string.
 * @param ...     Additional arguments for formatting.
 */
void task_console_printf(char *str_ptr, ...)
{
    console_buffer_t console_buffer;
    char *message_buffer;
    char *task_name;
    uint32_t length = 0;
    va_list args;

    /* ADD CODE */
    /* Allocate the message buffer */
    message_buffer = (char *) pvPortMalloc(CONSOLE_MAX_MESSAGE_LENGTH * sizeof(char));

    if (message_buffer)
    {
        va_start(args, str_ptr);
        task_name = pcTaskGetName(xTaskGetCurrentTaskHandle());
        length = snprintf(message_buffer, CONSOLE_MAX_MESSAGE_LENGTH, "%-16s : ",
                              task_name);

        vsnprintf((message_buffer + length), (CONSOLE_MAX_MESSAGE_LENGTH - length),
                  str_ptr, args);

        va_end(args);

        /* ADD CODE */
        /* Initialize the console buffer */
        console_buffer.data = message_buffer;
        console_buffer.index = (uint32_t)strlen(message_buffer);
        /* ADD CODE */
        /* The receiver task is responsible to free the memory from here on */
        xQueueSend(console_tx_queue, &console_buffer, portMAX_DELAY);


    }
    else
    {
        /* pvPortMalloc failed. Handle error */
        CY_ASSERT(0); // Halt the processor
    }
}
#endif