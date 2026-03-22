#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
    uint32_t gpr[32];    // r0 - r31
    uint32_t lr;         // Link Register
    uint32_t ctr;       // Count Register
    uint32_t cr;         // Condition Register
    uint8_t *ram;
    uint32_t (*rlwinm)(uint32_t, uint32_t, uint32_t, uint32_t);
} GekkoCPU;

// Byte-swapping helper for Windows
static inline uint32_t swap32(uint32_t val) {
    return ((val >> 24) & 0xff) |
           ((val << 8)  & 0xff0000) |
           ((val >> 8)  & 0xff00) |
           ((val << 24) & 0xff000000);
}

static inline uint8_t Read8(GekkoCPU *cpu, uint32_t addr) {
    // Basic physical mapping (Simplified for the recompiler)
    return *(uint8_t*)(cpu->ram + (addr & 0x01FFFFFF)); 
}

static inline void Write8(GekkoCPU *cpu, uint32_t addr, uint8_t val) {
    *(uint8_t*)(cpu->ram + (addr & 0x01FFFFFF)) = val;
}

static inline uint32_t __rlwinm(uint32_t val, uint32_t shift, uint32_t mb, uint32_t me) {
    // 1. Rotate Left
    uint32_t rotated = (val << shift) | (val >> (32 - shift));
    
    // 2. Create Mask
    uint32_t mask = 0;
    uint32_t m_start = mb;
    uint32_t m_end = me;
    
    if (m_start <= m_end) {
        for (int i = m_start; i <= m_end; i++) mask |= (1 << (31 - i));
    } else {
        // Wrapped mask
        for (int i = 0; i <= m_end; i++) mask |= (1 << (31 - i));
        for (int i = m_start; i <= 31; i++) mask |= (1 << (31 - i));
    }
    
    return rotated & mask;
}

#endif