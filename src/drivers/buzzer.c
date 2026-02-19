/**
 * @file buzzer.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "buzzer.h"
#include "cyhal_pwm.h"

static cyhal_pwm_t buzzer_pwm;
static float buzzer_duty;
static uint32_t buzzer_freq;

cy_rslt_t buzzer_init(float duty_cycle, uint32_t frequency)
{
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 100.0f) duty_cycle = 100.0f;

    buzzer_duty = duty_cycle;
    buzzer_freq = frequency;

    cy_rslt_t rslt = cyhal_pwm_init(&buzzer_pwm, PIN_BUZZER, NULL);
    if (rslt != CY_RSLT_SUCCESS) return rslt;

    rslt = cyhal_pwm_set_duty_cycle(&buzzer_pwm, 0.0f, buzzer_freq);
    if (rslt != CY_RSLT_SUCCESS) return rslt;

    rslt = cyhal_pwm_start(&buzzer_pwm);
    if (rslt != CY_RSLT_SUCCESS) return rslt;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t buzzer_start(void)
{
    cy_rslt_t rslt = cyhal_pwm_set_duty_cycle(&buzzer_pwm, buzzer_duty, buzzer_freq);
    if (rslt != CY_RSLT_SUCCESS) return rslt;
    return cyhal_pwm_start(&buzzer_pwm);
}

cy_rslt_t buzzer_stop(void)
{
    cyhal_pwm_stop(&buzzer_pwm);
    return cyhal_pwm_set_duty_cycle(&buzzer_pwm, 0.0f, buzzer_freq);
}
