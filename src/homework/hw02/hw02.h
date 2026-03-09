 /**
 * @file hw02.h
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-10-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
 #ifndef __HW02_H__
 #define __HW02_H__

 #include "main.h"

 #if defined(HW02)

#include "drivers.h"
#include "rtos_events.h"
#include "task_lcd.h"
#include "task_buttons.h"
#include "task_joystick.h"
#include "devices.h"
#include "master_mind_lib.h"

#define  TASK_SYSTEM_CONTROL_STACK_SIZE    (configMINIMAL_STACK_SIZE*5)
#define  TASK_SYSTEM_CONTROL_PRIORITY      (tskIDLE_PRIORITY + 1U)  

#define EVENT_JOYSTICK     (1 << 0) //Event bitmasks for ECE_HW02_Events event group.
#define EVENT_SW1          (1 << 1)
#define EVENT_SW2          (1 << 2) //Note switch 2 and 3 are not used in HW02
#define EVENT_SW3          (1 << 3)

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/
void task_hw02_system_control(void *pvParameters);


 #endif

 #endif /* __HW02_H__ */