#include <stdint.h>

static inline uint16_t rnval;
static inline uint16_t JOY_random(void) {
    rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
    return rnval;
}

static inline void JOY_setseed_default(void) { rnval = 0x1234; }

static inline void JOY_setseed(uint16_t seed) { rnval = seed; }