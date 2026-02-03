/**
 * @file timer.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2024-08-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "timer.h"
#include <complex.h>

cy_rslt_t timer_init(cyhal_timer_t *timer_obj, cyhal_timer_cfg_t *timer_cfg, uint32_t ticks, cyhal_timer_event_callback_t handler)
{
    cy_rslt_t rslt;

    timer_cfg->is_continuous = true;
    timer_cfg->direction = CYHAL_TIMER_DIR_UP;
    timer_cfg->is_compare = false;
    timer_cfg->period = ticks - 1u;
    timer_cfg->compare_value = 0u;
    timer_cfg->value = 0u;

    rslt = cyhal_timer_init(timer_obj, NC, NULL);
    if (rslt != CY_RSLT_SUCCESS) return rslt;

    rslt = cyhal_timer_set_frequency(timer_obj, 100000000u);
    if (rslt != CY_RSLT_SUCCESS) return rslt;

    rslt = cyhal_timer_configure(timer_obj, timer_cfg);
    if (rslt != CY_RSLT_SUCCESS) return rslt;

    cyhal_timer_register_callback(timer_obj, handler, NULL);
    cyhal_timer_enable_event(timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3u, true);

    return CY_RSLT_SUCCESS;
}

static void Ice3_timer_handler(void *callback_arg, cyhal_timer_event_t event)
{
    (void)callback_arg;
    (void)event;
}



