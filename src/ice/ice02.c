/**
 * @file ice02.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "main.h"

#if defined(ICE02)
#include "drivers.h"
#include "ece353-pins.h"
#include "lcd-io.h"

char APP_DESCRIPTION[] = "ECE353: ICE 02 - LCD 8080 Driver";

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void app_init_hw(void)
{
    cy_rslt_t rslt;

    console_init();
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
        printf("lcd_initialize failed: %ld\n\r", (long)rslt);
        while(1) {}
    }

    lcd_clear_screen(LCD_COLOR_BLACK);

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
    uint8_t minutes = 0;
    uint8_t seconds = 0;

    lcd_draw_time(minutes, seconds);

    while(1)
    {
        Cy_SysLib_Delay(1000);

        seconds++;
        if (seconds >= 60) { seconds = 0; minutes++; }
        if (minutes >= 100) { minutes = 0; }

        lcd_draw_time(minutes, seconds);
    }
}

#endif
