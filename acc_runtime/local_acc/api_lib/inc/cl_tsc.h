#ifndef _CL_TSC_H_
#define _CL_TSC_H_

#define CPU_FREQUENCY (3600)
#include <stdint.h>
inline uint64_t ticks() {
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

inline double cycles_to_nanoseconds(uint64_t cycles) {
    return (uint64_t)((double) cycles / CPU_FREQUENCY * 1000);
}

inline double cycles_to_microseconds(uint64_t cycles) {
    return cycles_to_nanoseconds(cycles) / 1000;
}

inline double cycles_to_milliseconds(uint64_t cycles) {
    return cycles_to_nanoseconds(cycles) / 1000000;
}

inline double cycles_to_seconds(uint64_t cycles) {
    return cycles_to_nanoseconds(cycles) / 1000000000;
}

#endif //_CL_TSC_H_
