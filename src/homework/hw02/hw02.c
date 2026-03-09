 /**
 * @file hw02.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-10-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "hw02.h"

#if defined(HW02)

char APP_DESCRIPTION[] = "ECE353 S26 HW02";

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
EventGroupHandle_t ECE353_RTOS_Events;
EventBits_t HW02_Events_Bits;

extern QueueHandle_t Queue_Joystick;
QueueHandle_t Queue_Response_SystemControl;
QueueHandle_t Queue_Response_LCD;
QueueHandle_t Queue_LCD;


/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/
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

static void draw_cipher_tile(uint8_t col, uint8_t value, bool active)
{
    lcd_msg_t msg;

    msg.command = active ? LCD_CMD_DRAW_TILE : LCD_CMD_DRAW_TILE_INVERTED;
    msg.payload.tile.row = LCD_TILE_ROW_CYPHER;
    msg.payload.tile.col = col;
    msg.payload.tile.number = value;
    msg.payload.tile.color_fg = LCD_COLOR_RED;
    msg.payload.tile.color_bg = LCD_COLOR_BLACK;

    xQueueSend(Queue_LCD, &msg, portMAX_DELAY);
}


void task_hw02_system_control(void *pvParameters)
{
    (void)pvParameters; // Unused parameter   
    /* Draw the initial master mind game board */
    //Draw string "select your cypher" atop the lcd screen
    //Then draw starting tile lay that is 4 0's in red and 0-7 in green, see hw01 commit for reference:

        /* Allocate a lcd_msg_t variable */
    lcd_msg_t msg;

        /*State variables*/
    uint8_t active_input_num = 0;
    uint8_t active_cipher_col = 0;
    bool joystick_centered = true;
    bool done = false;
    uint8_t cipher[4] = {0, 0, 0, 0};
    
    /* Write a message to the user in the Text Area of the screen*/
    msg.command = LCD_CMD_PRINT_MESSAGE;
    snprintf(msg.payload.message, 32, "Select Your Cypher!");
    master_mind_handle_msg(&msg);


    /* Draw 4 blank tiles for the secret code */
    for(int col = 0; col < 4; col++)
    {
        msg.command = LCD_CMD_DRAW_TILE;
        msg.payload.tile.row = LCD_TILE_ROW_CYPHER;
        msg.payload.tile.col = col;
        msg.payload.tile.number = 0; // number is ignored for code tiles
        msg.payload.tile.color_fg = LCD_COLOR_RED;
        msg.payload.tile.color_bg = LCD_COLOR_BLACK;
        master_mind_handle_msg(&msg);
    }


    /* Draw numbers 0-3 for the user input*/
    for(int col = 0; col < 4; col++)
    {
        msg.command = LCD_CMD_DRAW_TILE_INVERTED;
        msg.payload.tile.row = LCD_TILE_ROW_NUM_0_3;
        msg.payload.tile.col = col;
        msg.payload.tile.number = col;
        msg.payload.tile.color_fg = LCD_COLOR_GREEN;
        msg.payload.tile.color_bg = LCD_COLOR_BLACK;

        master_mind_handle_msg(&msg);
    }

    /* Draw the numbers for 4-7 for the user input*/
    for(int col = 0; col < 4; col++)
    {
        msg.command = LCD_CMD_DRAW_TILE_INVERTED;
        msg.payload.tile.row = LCD_TILE_ROW_NUM_4_7;
        msg.payload.tile.col = col;
        msg.payload.tile.number = col + 4;
        msg.payload.tile.color_fg = LCD_COLOR_GREEN;
        msg.payload.tile.color_bg = LCD_COLOR_BLACK;
        master_mind_handle_msg(&msg);
    }

    /* Allow the user to select 4 numbers for their cypher */

    while(1)
        {
            HW02_Events_Bits = xEventGroupWaitBits(
                ECE353_RTOS_Events,
                EVENT_JOYSTICK | EVENT_SW1 | EVENT_SW2 | EVENT_SW3,
                pdTRUE,
                pdFALSE,
                portMAX_DELAY
            );

            if (HW02_Events_Bits & EVENT_JOYSTICK)
            {
                joystick_position_t pos;
                xQueuePeek(Queue_Joystick, &pos, 0);

                if (pos == JOYSTICK_POS_CENTER)
                {
                    joystick_centered = true;
                }
                else if (joystick_centered && !done)
                {
                    uint8_t old_input_num = active_input_num;

                    switch(pos)
                    {
                        case JOYSTICK_POS_LEFT:
                            if ((active_input_num % 4) != 0) active_input_num--;
                            break;

                        case JOYSTICK_POS_RIGHT:
                            if ((active_input_num % 4) != 3) active_input_num++;
                            break;

                        case JOYSTICK_POS_UP:
                            if (active_input_num >= 4) active_input_num -= 4;
                            break;

                        case JOYSTICK_POS_DOWN:
                            if (active_input_num <= 3) active_input_num += 4;
                            break;

                        default:
                            break;
                    }

                    if (active_input_num != old_input_num)
                    {
                        draw_input_tile(old_input_num, false);
                        draw_input_tile(active_input_num, true);
                    }

                    joystick_centered = false;
                }
            }

            if ((HW02_Events_Bits & EVENT_SW1) && !done)
            {
                cipher[active_cipher_col] = active_input_num;

                draw_cipher_tile(active_cipher_col, active_input_num, false);

                active_cipher_col++;

                if (active_cipher_col < 4)
                {
                    draw_cipher_tile(active_cipher_col, 0, true);
                }
                else
                {
                    done = true;
                }
            }
        }
}


/*************************************************
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 ************************************************/
void app_init_hw(void)
{
    cy_rslt_t rslt;

    console_init();
    // Set text color to black
    printf("\x1b[30m");
    printf("\x1b[2J\x1b[;H");
    printf("**************************************************\n\r");
    printf("* %s\n\r", APP_DESCRIPTION);
    printf("* Date: %s\n\r", __DATE__);
    printf("* Time: %s\n\r", __TIME__);
    printf("* Name:%s\n\r", NAME);
    printf("**************************************************\n\r");

    rslt = lcd_initialize();
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("LCD initialization failed!\n\r");
        for(int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

    /* Initialize the joystick */
    joystick_init();

    /* Initialize the buttons*/
    buttons_init_gpio();

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
    /* Create the event group for RTOS events */
    ECE353_RTOS_Events = xEventGroupCreate();

    if (ECE353_RTOS_Events == NULL)
    {
        printf("Failed to create event group!\n\r");
        for(int i = 0; i < 100000; i++) {}
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

    /* Create the FreeRTOS tasks */
    if (!task_joystick_init())
    {
        printf("Failed to create joystick queue!\n\r");
        for(int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

     if (!task_buttons_init())
    {
        printf("Failed to create buttons queue!\n\r");
        for(int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

     if (!task_lcd_resources_init(Queue_LCD))
    {
        printf("Failed to create LCD queue!\n\r");
        for(int i = 0; i < 100000; i++) {}
        CY_ASSERT(0);
    }

     xTaskCreate(task_hw02_system_control, 
        "System Control", 
        TASK_SYSTEM_CONTROL_STACK_SIZE, 
        NULL, 
        TASK_SYSTEM_CONTROL_PRIORITY, 
        NULL
    );

    /* Start the scheduler*/
    vTaskStartScheduler();

    /* Will never reach this loop once the scheduler starts */
    while (1)
    {
        printf("you shouldn't see this.");
    }
}
#endif