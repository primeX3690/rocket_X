


#include "__init_hardware.h"
#include <stdint.h>

typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
    volatile uint32_t CPAR; // Peripheral Address
    volatile uint32_t CMAR; // Memory Address
    volatile uint32_t CNDTR; // Number of Data Items
    volatile uint32_t CCR;  // Control Register
} DMA_Channel_Def_t;

#define DMA1_CHANNEL1 ((DMA_Channel_Def_t*)DMA1_BASE_ADDR)

void dma_transfer_init(uint32_t* dest_memory_buffer, uint32_t buffer_size) {
    // Configure DMA stream from SPI Receive Register to RAM Buffer
    // DMA1_CHANNEL1->CPAR = SPI1_BASE_ADDR + 0x08; // SPI Data Register offset
    // DMA1_CHANNEL1->CMAR = (uint32_t)dest_memory_buffer;
    // DMA1_CHANNEL1->CNDTR = buffer_size;
    // DMA1_CHANNEL1->CCR = 0x2092; // Circular mode, memory increment enabled
    (void)dest_memory_buffer;
    (void)buffer_size;
}

void dma_start_transfer(void) {
    // DMA1_CHANNEL1->CCR |= 0x0001; // Enable DMA Channel
}