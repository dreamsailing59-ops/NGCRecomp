#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
    uint32_t gpr[32];    // r0 - r31
    uint32_t lr;         // Link Register
    uint32_t ctr;        // Count Register
    uint8_t  ram[0x01800000]; // 24MB GameCube RAM
} GekkoCPU;

// Byte-swapping helper for Windows
static inline uint32_t swap32(uint32_t val) {
    return ((val >> 24) & 0xff) |
           ((val << 8)  & 0xff0000) |
           ((val >> 8)  & 0xff00) |
           ((val << 24) & 0xff000000);
}

// Write 32-bit word to Virtual RAM
static inline void Write32(GekkoCPU *cpu, uint32_t addr, uint32_t val) {
    // GameCube RAM is usually accessed via 0x80XXXXXX
    // Masking the top bits to fit our 24MB buffer
    uint32_t physicalAddr = addr & 0x01FFFFFF;
    
    // Swap to Big-Endian before storing
    uint32_t swapped = swap32(val);
    *(uint32_t*)(&cpu->ram[physicalAddr]) = swapped;
}

#endif