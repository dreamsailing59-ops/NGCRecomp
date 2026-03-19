#include <stdint.h>

typedef struct {
    uint32_t gpr[32];    // r0 - r31
    uint32_t lr;         // Link Register
    uint32_t ctr;        // Count Register
    uint32_t pc;         // Program Counter
    uint8_t  mem[0x01800000]; // 24MB Virtual RAM
} GekkoCPU;