


#include "__init_hardware.h"
#include <stdint.h>

typedef struct {
    volatile uint32_t TIM_CR1;
    volatile uint32_t TIM_PSC;
    volatile uint32_t TIM_ARR;
    volatile uint32_t TIM_CCR1; // Channel 1 (Pitch)
    volatile uint32_t TIM_CCR2; // Channel 2 (Yaw)
    volatile uint32_t TIM_CCR3; // Channel 3 (Roll)
} PWM_Timer_Def_t;

#define PWM_TIMER ((PWM_Timer_Def_t*)PWM_BASE_ADDR)

void pwm_actuator_init(void) {
    // Configure PWM timer frequency to 400Hz servo update rate
    // PWM_TIMER->TIM_PSC = 71; // Prescaler
    // PWM_TIMER->TIM_ARR = 2499; // Auto-reload register for 400Hz
    // PWM_TIMER->TIM_CR1 = 0x0001; // Enable timer
}

void pwm_set_actuator_deflections(double delta_pitch, double delta_yaw, double delta_roll) {
    // Map radians (-0.5 to +0.5) to PWM duty cycle counts (e.g., 1000us to 2000us pulse width)
    uint32_t pitch_ticks = (uint32_t)(1500.0 + (delta_pitch * 500.0 / 0.5));
    uint32_t yaw_ticks   = (uint32_t)(1500.0 + (delta_yaw * 500.0 / 0.5));
    uint32_t roll_ticks  = (uint32_t)(1500.0 + (delta_roll * 500.0 / 0.5));

    // Write directly to timer capture/compare registers
    // PWM_TIMER->TIM_CCR1 = pitch_ticks;
    // PWM_TIMER->TIM_CCR2 = yaw_ticks;
    // PWM_TIMER->TIM_CCR3 = roll_ticks;
    (void)pitch_ticks;
    (void)yaw_ticks;
    (void)roll_ticks;
}