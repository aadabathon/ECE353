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
#include "main.h"

#ifdef ECE353_FREERTOS
#include "drivers.h"
#include "task_console.h"
#include "cyhal_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

/**
 * @brief
 * This file contains the implementation of the console receive (Rx) task.
 * The task is responsible for processing incoming console commands and
 * controlling the state of the LEDs accordingly.
 * 
 * The task uses a double buffer to process the incoming console commands.
 * The supported commands will be "RED_ON" and "RED_OFF" to control the red LED.
 */
/* Global Variables */
console_buffer_t console_buffer1;
console_buffer_t console_buffer2;

//Pointers
volatile console_buffer_t *produce_console_buffer;
volatile console_buffer_t *consume_console_buffer;

//Task Handles
TaskHandle_t TaskHandle_Console_Rx;

/**
 * @brief
 * This function is the bottom half task for receiving console input.
 *
 * It waits for a task notification from the ISR indicating that a new 
 * command has been received. The task then processes the command and 
 * controls the state of the LEDs accordingly.
 *
 * @param param Unused parameter
 */
void task_console_rx(void *param)
{
    (void)param;

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        const char *cmd = (const char *)consume_console_buffer->data;

        if (strcmp(cmd, "RED ON") == 0)
        {
            cyhal_gpio_toggle(PIN_LED_RED);
            cyhal_gpio_write(PIN_LED_RED, 0);   // active-low ON
            task_console_printf("Red LED is ON\r\n");
        }
        else if (strcmp(cmd, "RED OFF") == 0)
        {
            cyhal_gpio_write(PIN_LED_RED, 1);   // active-low OFF
            task_console_printf("Red LED is OFF\r\n");
        }
        else
        {
            task_console_printf("Invalid Command: %s\r\n", cmd);
        }

        consume_console_buffer->index = 0;
        consume_console_buffer->data[0] = '\0';
    }
}


/**
 * @brief
 * This function initializes the resources for the console Rx task.
 * @return true if resources were initialized successfully
 * @return false if resource initialization failed
 */
bool task_console_resources_init_rx(void)
{
    BaseType_t rslt;

    //Allocate an array from the heap for buffers:
    console_buffer1.data = (char *)pvPortMalloc(CONSOLE_MAX_MESSAGE_LENGTH * sizeof(char));
    console_buffer2.data = (char *)pvPortMalloc(CONSOLE_MAX_MESSAGE_LENGTH * sizeof(char));

    if ((console_buffer1.data == NULL) || (console_buffer2.data == NULL))
    {
        return false;
    }

    memset(console_buffer1.data, 0, CONSOLE_MAX_MESSAGE_LENGTH);
    memset(console_buffer2.data, 0, CONSOLE_MAX_MESSAGE_LENGTH);
    
    produce_console_buffer = &console_buffer1;
    consume_console_buffer = &console_buffer2;

    //Set indexes
    console_buffer1.index = 0;
    console_buffer2.index = 0;

    //Task init
    rslt = xTaskCreate( //BOTTOM HALF TASK, SEE WEBSITE FOR INFO
        task_console_rx,          // Task function
        "Console Rx Task",       // Name of the task (for debugging)
        configMINIMAL_STACK_SIZE, // Stack size in words
        NULL,                    // Task parameter
        tskIDLE_PRIORITY + 1,    // Task priority
        &TaskHandle_Console_Rx   // Task handle
    );
    //done
    return (rslt == pdPASS); // Resources initialized successfully
}
#endif