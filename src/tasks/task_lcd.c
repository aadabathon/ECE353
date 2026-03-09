/**
 * @file task_lcd.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief FreeRTOS task for handling LCD draw requests.
 *
 * This task acts as a simple LCD renderer. It blocks on a queue of lcd_msg_t
 * messages and forwards each received message to master_mind_handle_msg().
 *
 * In HW02, higher-level application logic decides what to draw, and this task
 * is only responsible for receiving queued LCD requests and executing them.
 *
 * @version 0.1
 * @date 2025-08-18
 *
 * @copyright Copyright (c) 2025
 */

#include "task_lcd.h"

#if defined(ECE353_FREERTOS)

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/* Queue used to receive LCD draw/update requests */
static QueueHandle_t Queue_Requests = NULL;

/*****************************************************************************/
/* Task Definitions                                                          */
/*****************************************************************************/

/**
 * @brief LCD task that processes queued LCD messages.
 *
 * This task blocks indefinitely waiting for lcd_msg_t requests. Whenever a
 * message is received, it is passed directly to master_mind_handle_msg(),
 * which performs the actual LCD update.
 *
 * @param pvParameters
 * Unused task parameter.
 */
void task_lcd(void *pvParameters)
{
    (void)pvParameters;

    lcd_msg_t msg;

    while (1)
    {
        /*
         * Wait indefinitely for the next LCD request.
         * Each message fully describes one LCD operation.
         */
        if (xQueueReceive(Queue_Requests, &msg, portMAX_DELAY) == pdPASS)
        {
            master_mind_handle_msg(&msg);
        }
    }
}

/*****************************************************************************/
/* Initialization                                                            */
/*****************************************************************************/

/**
 * @brief Initialize LCD task resources and create the LCD task.
 *
 * This function stores the queue handle that the LCD task will listen on,
 * then creates the FreeRTOS task responsible for processing LCD messages.
 *
 * @param queue_request
 * Queue handle used to send lcd_msg_t requests to the LCD task.
 *
 * @return true if initialization succeeded
 * @return false if the provided queue is NULL or task creation fails
 */
bool task_lcd_resources_init(QueueHandle_t queue_request)
{
    BaseType_t result;

    /* The LCD task requires a valid request queue */
    if (queue_request == NULL)
    {
        return false;
    }

    Queue_Requests = queue_request;

    /* Create the LCD task */
    result = xTaskCreate(
        task_lcd,
        "LCD Task",
        TASK_LCD_STACK_SIZE,
        NULL,
        TASK_LCD_PRIORITY,
        NULL
    );

    if (result != pdPASS)
    {
        return false;
    }

    return true;
}

#endif