


#ifndef INIT_HARDWARE_H
#define INIT_HARDWARE_H

#include <stdint.h>

// Simulated Memory-Mapped Register Base Addresses for ARM Cortex / FPGA
#define PERIPHERAL_BASE       0x40000000UL
#define SPI1_BASE_ADDR        (PERIPHERAL_BASE + 0x3000UL)
#define PWM_BASE_ADDR         (PERIPHERAL_BASE + 0x8000UL)
#define DMA1_BASE_ADDR        (PERIPHERAL_BASE + 0x6000UL)

// Hardware Status Codes
typedef enum {
    HW_OK = 0,
    HW_ERROR_SPI_TIMEOUT = 1,
    HW_ERROR_DMA_FAULT = 2
} HardwareStatus;

// Initialization functions
HardwareStatus init_hardware_registers(void);
void system_clock_config_5khz(void);

#endif // INIT_HARDWARE_H