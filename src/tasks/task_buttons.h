/**
 * @file task_buttons.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-08-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
 #ifndef __TASK_BUTTONS_H__
 #define __TASK_BUTTONS_H__

 #include "main.h"

#ifdef ECE353_FREERTOS

 #include "drivers.h"
 #include "rtos_events.h"

#define EVENT_SW1          (1 << 1)
#define EVENT_SW2          (1 << 2) //Note switch 2 and 3 are not used in HW02
#define EVENT_SW3          (1 << 3)

typedef struct {
    uint8_t stable;
    uint8_t last_raw;
    uint8_t cnt;
    } debounce_t;
 
void debounce_update(debounce_t *d, uint8_t raw, uint8_t threshold, bool *pressed_edge);
void task_buttons(void *arg);
bool task_buttons_init(void);
 #endif

#endif // __TASK_BUTTONS_H__
