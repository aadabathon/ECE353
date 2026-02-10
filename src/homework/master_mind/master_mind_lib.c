/**
 * @file master_mind_lib.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2026-01-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "main.h"
#include "drivers.h"
#include "master_mind_lib.h"
#include "lcd-io.h"
#include "lcd-fonts.h"


/**
 * @brief 
 * This function will parse an LCD message and perform the appropriate action
 * @param msg 
 * @return true 
 * @return false 
 */

//Helper functions used in master_mind_handle_msg to perform the appropriate action for each command
/**
* @brief 
* lcd_draw_tile is a helper function that will draw a tile on the LCD screen based on the information in the tile struct
* @param tile
* @return true if the tile was drawn successfully, false if there was an error (e.g. null pointer)
*/

bool lcd_draw_tile(lcd_tile_t* tile){
    if(tile == NULL){ // Check for null pointer, return false if null
        return false;
    } 
    lcd_rect_t rect; // Create a rect struct to hold the coordinates and dimensions of the tile
    if(!lcd_tile_rect(&rect, tile->row, tile->col)){ // Check if the tile rect is valid, return false if not
        return false;
    }
    lcd_draw_rectangle(rect.cx, rect.cy, rect.w, rect.h, tile->color_bg, true); // Draw the filled rectangle for the tile and return the result
     //if (tile->row != LCD_TILE_ROW_CYPHER)
    //{ 
    if(tile->number > 7){ // Check for valid number for code tiles, return false if invalid
        return false;
    }
    char c = tile->number + '0'; // Convert the number in the tile struct to a character by adding '0' to it
    uint16_t index = c - '0'; // Calculate the index of the character in the font bitmap by subtracting the start character from the character
    
    FONT_CHAR_INFO char_info = FONT_CHAR_INFO_LARGE_NUMBERS[index]; // Get the character information from the font info struct using the index
    const uint8_t *bitmap = FONT_NUM_LARGE_BITMAPS + char_info.offset;
    lcd_draw_image(rect.cx, rect.cy, char_info.width, char_info.height,
    bitmap, tile->color_fg, tile->color_bg, true); // Draw the background color for the tile and return the result
    //}
    return true;
}

/**
* @brief 
* lcd_draw_tile_inverted is a helper function that will draw an inverted tile on the LCD screen based on the information in the tile struct
* @param tile
* @return true if the tile was drawn successfully, false if there was an error (e.g. null pointer)
*/
bool lcd_draw_tile_inverted(lcd_tile_t* tile){
    if (tile == NULL){ // Check for null pointer, return false if null
        return false;
    }
    lcd_rect_t rect; // Create a rect struct to hold the coordinates and dimensions of the tile
    if(!lcd_tile_rect(&rect, tile->row, tile->col)){ // Check if the tile rect is valid, return false if not
        return false;
    }
    lcd_draw_rectangle(rect.cx, rect.cy, rect.w, rect.h, tile->color_bg, true); // Draw the filled rectangle for the tile and return the result
    //if (tile->row != LCD_TILE_ROW_CYPHER)
    //{ 
    if(tile->number > 7){ // Check for valid number for code tiles, return false if invalid
        return false;
    }
    char c = tile->number + '0'; // Convert the number in the tile struct to a character by adding '0' to it
    uint16_t index = c - '0'; // Calculate the index of the character in the font bitmap by subtracting the start character from the character
    
    FONT_CHAR_INFO char_info = FONT_CHAR_INFO_LARGE_NUMBERS[index]; // Get the character information from the font info struct using the index
    const uint8_t *bitmap = FONT_NUM_LARGE_BITMAPS + char_info.offset;
    lcd_draw_image(rect.cx, rect.cy, char_info.width, char_info.height,
    bitmap, tile->color_bg, tile->color_fg, true); // Draw the background color for the tile and return the result
    //}// //note that this helper function differs from lcd_draw_tile in that the foreground and background colors are swapped in our draw_image() call
    return true;
};
/**
* @brief 
* lcd_print_message is a helper function that will print a message on the LCD screen based on the information in the message string
* @param tile
* @return true if the message was dispaly on the LCD screen successfully, false if there was an error (e.g. null pointer)
*/
bool lcd_print_message(char* message){
    if (message == NULL){ // Check for null pointer, return false if null
        return false;
    }
    uint16_t x = LCD_MARGIN; //Set the x coordinate to the left margin
    uint16_t y = lcd_text_area_center_y(); // Set the y coordinate to the center of the text area using the helper function we defined in master_mind_lib.h

    for (int i =0; message[i] != '\0'; i++){
        char c = message[i];
        uint16_t index = c - Consolas_20ptFontInfo.start_char;
        FONT_CHAR_INFO char_info = Consolas_20ptDescriptors[index];
        const uint8_t *bitmap = Consolas_20ptBitmaps + char_info.offset;

        lcd_draw_image(
            x,
            y,
            char_info.width,
            char_info.height,
            bitmap,
            LCD_COLOR_WHITE,
            LCD_COLOR_BLACK,
            true
        );
        x += char_info.width; // Move the x coordinate to the right for the next character
    } 
    
}

//====================================================================================================
// THEE mastermind function for HW01 

bool master_mind_handle_msg(lcd_msg_t* msg)
{
    if(msg == NULL) // Check for null pointer, return false if null
    {
        return false;
    } 

    switch(msg->command) //For our message struct, we will switch on the command field to determine which helper to call 
    {
        case LCD_CMD_DRAW_TILE: // If the command is LCD_CMD_DRAW_TILE, we will call the lcd_draw_tile helper function and pass in the tile
            return lcd_draw_tile(&msg->payload.tile);
        case LCD_CMD_DRAW_TILE_INVERTED: // If the command is LCD_CMD_DRAW_TILE_INVERTED, we will call the lcd_draw_tile_inverted helper function and pass in the tile
            return lcd_draw_tile_inverted(&msg->payload.tile);
        case LCD_CMD_PRINT_MESSAGE: // If the command is LCD_CMD_PRINT_MESSAGE, we will call the lcd_print_message helper function and pass in the message
            return lcd_print_message(msg->payload.message);
        case LCD_CMD_CLEAR_SCREEN: // If the command is LCD_CMD_CLEAR_SCREEN, we will call the lcd_clear_screen helper function and pass in the color black
             lcd_clear_screen(LCD_COLOR_BLACK);
             return true;
        default:
            break; // If no case is hit we break the switch statement and return false
    }
    return false;
}