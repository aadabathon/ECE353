/**
 * @file hw02.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief ECE353 HW02 application code for Mastermind cipher selection.
 *
 * This file contains the top-level application initialization, RTOS object
 * creation, and the system control task that manages the cipher-selection
 * workflow. The system control task owns the selection state and updates
 * the LCD through helper draw functions and the LCD queue.
 *
 * @version 0.1
 * @date 2025-10-08
 *
 * @copyright Copyright (c) 2025
 */
#include "hw02.h"

#if defined(HW02)

char APP_DESCRIPTION[] = "ECE353 S26 HW02";

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/* Shared RTOS event group used by the application tasks */
EventGroupHandle_t ECE353_RTOS_Events;
EventBits_t HW02_Events_Bits;

/* Queue written by the joystick task and read by system control */
extern QueueHandle_t Queue_Joystick;

/* Application queues */
QueueHandle_t Queue_Response_SystemControl;
QueueHandle_t Queue_Response_LCD;
QueueHandle_t Queue_LCD;

/*****************************************************************************/
/* Helper Functions                                                          */
/*****************************************************************************/

/**
 * @brief Draw one selectable input tile (numbers 0-7).
 *
 * @param num
 * The number to draw.
 *
 * @param active
 * true  -> draw normal tile appearance
 * false -> draw inverted tile appearance to show current selection
 */
static void draw_input_tile(uint8_t num, bool active)
{
    lcd_msg_t msg;

    msg.command = active ? LCD_CMD_DRAW_TILE : LCD_CMD_DRAW_TILE_INVERTED;
    msg.payload.tile.row = (num < 4) ? LCD_TILE_ROW_NUM_0_3 : LCD_TILE_ROW_NUM_4_7;
    msg.payload.tile.col = num % 4;
    msg.payload.tile.number = num;
    msg.payload.tile.color_fg = LCD_COLOR_GREEN;
    msg.payload.tile.color_bg = LCD_COLOR_BLACK;

    xQueueSend(Queue_LCD, &msg, portMAX_DELAY);
}

/**
 * @brief Draw one cipher tile in the top row.
 *
 * @param col
 * Cipher column to update (0-3).
 *
 * @param value
 * Number to display in that cipher tile.
 *
 * @param active
 * true  -> draw normal tile appearance
 * false -> draw inverted tile appearance to show active cipher slot
 */
static void draw_cipher_tile(uint8_t col, uint8_t value, bool active)
{
    lcd_msg_t msg;

    msg.command = active ? LCD_CMD_DRAW_TILE : LCD_CMD_DRAW_TILE_INVERTED;
    msg.payload.tile.row = LCD_TILE_ROW_CYPHER;
    msg.payload.tile.col = col;
    msg.payload.tile.number = value;
    msg.payload.tile.color_fg = LCD_COLOR_BLACK;
    msg.payload.tile.color_bg = LCD_COLOR_RED;

    xQueueSend(Queue_LCD, &msg, portMAX_DELAY);
}

/**
 * @brief Draw the initial Mastermind board.
 *
 * This draws:
 * - the "Select Your Cypher!" message
 * - the 4 cipher tiles across the top row
 * - the 8 selectable input tiles across the bottom two rows
 *
 * The initial active cipher slot is column 0, and the initial active input
 * number is 0.
 */
static void draw_initial_board(void)
{
    lcd_msg_t msg;

    /* Write startup message in the LCD text area */
    msg.command = LCD_CMD_PRINT_MESSAGE;
    snprintf(msg.payload.message, 32, "Select Your Cypher!");
    master_mind_handle_msg(&msg);

    /* Draw cipher row: first tile active, remaining tiles inactive */
    draw_cipher_tile(0, 0, true);

    for (int col = 1; col < 4; col++)
    {
        draw_cipher_tile(col, 0, false);
    }

    /* Draw selectable input row: number 0 active, all others inactive */
    draw_input_tile(0, false);

    for (int num = 1; num < 8; num++)
    {
        draw_input_tile(num, true);
    }
}

/*****************************************************************************/
/* Task Definitions                                                          */
/*****************************************************************************/

/**
 * @brief System control task for HW02.
 *
 * This task is the main state-machine "brain" of the cipher-selection
 * workflow. It owns the currently selected input number, the active cipher
 * column, and the final 4-number cipher. It waits on joystick and SW1 events,
 * then updates application state and sends LCD draw commands as needed.
 *
 * Behavior:
 * - Joystick movement changes the currently highlighted input tile
 * - SW1 commits the selected input into the active cipher slot
 * - After 4 selections, input is complete and further updates stop
 */
void task_hw02_system_control(void *pvParameters)
{
    (void)pvParameters;

    /* State variables owned by this task */
    uint8_t active_input_num = 0;      /* Currently highlighted selectable number */
    uint8_t active_cipher_col = 0;     /* Current cipher column being filled */
    bool joystick_centered = true;     /* Require return-to-center between moves */
    bool done = false;                 /* True after all 4 cipher digits are chosen */
    uint8_t cipher[4] = {0, 0, 0, 0};  /* Final selected cipher */

    /* Draw the initial game board */
    draw_initial_board();

    /* Allow the user to select 4 numbers for their cipher */
    while (1)
    {
        HW02_Events_Bits = xEventGroupWaitBits(
            ECE353_RTOS_Events,
            EVENT_JOYSTICK | EVENT_SW1,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );

        printf("System woke: 0x%08lx\r\n", HW02_Events_Bits);

        /*********************************************************************/
        /* Handle joystick movement                                           */
        /*********************************************************************/
        if (HW02_Events_Bits & EVENT_JOYSTICK)
        {
            joystick_position_t pos;
            xQueuePeek(Queue_Joystick, &pos, 0);

            /*
             * Only allow one movement per joystick deflection.
             * The joystick must return to center before another movement
             * is accepted.
             */
            if (pos == JOYSTICK_POS_CENTER)
            {
                joystick_centered = true;
            }
            else if (joystick_centered && !done)
            {
                uint8_t old_input_num = active_input_num;

                switch (pos)
                {
                    case JOYSTICK_POS_LEFT:
                        if ((active_input_num % 4) != 0)
                        {
                            active_input_num--;
                        }
                        break;

                    case JOYSTICK_POS_RIGHT:
                        if ((active_input_num % 4) != 3)
                        {
                            active_input_num++;
                        }
                        break;

                    case JOYSTICK_POS_UP:
                        if (active_input_num >= 4)
                        {
                            active_input_num -= 4;
                        }
                        break;

                    case JOYSTICK_POS_DOWN:
                        if (active_input_num <= 3)
                        {
                            active_input_num += 4;
                        }
                        break;

                    default:
                        break;
                }

                /* Redraw only if the highlighted input actually changed */
                if (active_input_num != old_input_num)
                {
                    draw_input_tile(old_input_num, true);
                    draw_input_tile(active_input_num, false);
                }

                joystick_centered = false;
            }
        }

        /*********************************************************************/
        /* Handle SW1 selection                                               */
        /*********************************************************************/
        if ((HW02_Events_Bits & EVENT_SW1) && !done)
        {
            /* Commit the currently selected input number to the active slot */
            cipher[active_cipher_col] = active_input_num;

            /* Redraw the selected cipher tile with the chosen value */
            draw_cipher_tile(active_cipher_col, active_input_num, false);

            /* Advance to the next cipher position */
            active_cipher_col++;

            if (active_cipher_col < 4)
            {
                /* Highlight the next empty cipher slot */
                draw_cipher_tile(active_cipher_col, 0, true);
            }
            else
            {
                /* All 4 numbers have been selected */
                done = true;
            }
        }
    }
}

/*************************************************
 * @brief
 * Initialize all required hardware resources for
 * the ICE platform.
 ************************************************/
void app_init_hw(void)
{
    cy_rslt_t rslt;

    console_init();

    /* Clear terminal and print application banner */
    printf("\x1b[0m");
    printf("\x1b[2J\x1b[;H");
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

    /* Initialize LCD hardware */
    rslt = lcd_initialize();
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("LCD initialization failed!\n\r");
        for (int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    /* Initialize joystick and button GPIO hardware */
    joystick_init();
    buttons_init_gpio();
}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/

/**
 * @brief
 * Create RTOS objects, create tasks, and start the scheduler.
 */
void app_main(void)
{
    /* Create the event group for RTOS events */
    ECE353_RTOS_Events = xEventGroupCreate();

    if (ECE353_RTOS_Events == NULL)
    {
        printf("Failed to create event group!\n\r");
        for (int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    /* Create the FreeRTOS queues */
    Queue_Response_SystemControl = xQueueCreate(1, sizeof(device_response_msg_t));
    if (Queue_Response_SystemControl == NULL)
    {
        printf("Failed to create system control response queue!\n\r");
        CY_ASSERT(0);
    }

    Queue_LCD = xQueueCreate(8, sizeof(lcd_msg_t));
    if (Queue_LCD == NULL)
    {
        printf("Failed to create LCD queue!\n\r");
        CY_ASSERT(0);
    }

    /* Create the FreeRTOS tasks and their resources */
    if (!task_joystick_init())
    {
        printf("Failed to create joystick queue!\n\r");
        for (int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    if (!task_buttons_init())
    {
        printf("Failed to create buttons queue!\n\r");
        for (int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    if (!task_lcd_resources_init(Queue_LCD))
    {
        printf("Failed to create LCD queue!\n\r");
        for (int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    xTaskCreate(
        task_hw02_system_control,
        "System Control",
        TASK_SYSTEM_CONTROL_STACK_SIZE,
        NULL,
        TASK_SYSTEM_CONTROL_PRIORITY,
        NULL
    );

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /* Execution should never reach here */
    while (1)
    {
        printf("you shouldn't see this.");
    }
}

#endif