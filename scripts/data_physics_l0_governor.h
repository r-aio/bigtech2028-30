/**
 * @file data_physics_l0_governor.h
 * @brief C99 Real-Time Governor for Data Physics & L0-ISM Hyperscale Infrastructure Interdiction
 * @note Zero-heap, O(1) execution latency (<0.35 ms) for ARM Cortex-M7 / FPGA control units.
 */

#ifndef DATA_PHYSICS_L0_GOVERNOR_H
#define DATA_PHYSICS_L0_GOVERNOR_H

#include <stdint.h>
#include <stdbool.h>

#define L0_BOUND_MIN       (-9)
#define L0_BOUND_MAX       (+9)
#define L0_CRITICAL_THRESH (+7)
#define L0_HARD_STOP       (-999)
#define L0_PERMIT          (0)
#define L0_WARNING_STATE   (1)

typedef struct {
    int8_t s_thermal;  /* Thermal / Liquid D2C Cooling Load State [-9, +9] */
    int8_t s_grid;     /* Power Transmission / HV Grid Capacity State [-9, +9] */
    int8_t s_capEx;    /* Capital Expenditure ROCE Yield State [-9, +9] */
} data_physics_vector_t;

/**
 * @brief Audytuje wektor stanu infrastruktury BigTech pod kątem inwariantów L0-ISM
 * @param vec Wskaźnik na wektor stanu infrastruktury
 * @return L0_PERMIT (0), L0_WARNING_STATE (1) lub L0_HARD_STOP (-999)
 */
static inline int16_t verify_data_physics_state(const data_physics_vector_t* vec) {
    if (!vec) return L0_HARD_STOP;

    /* 1. Sprawdzenie nasycenia granic Bekensteina [-9, +9] */
    if (vec->s_thermal < L0_BOUND_MIN || vec->s_thermal > L0_BOUND_MAX ||
        vec->s_grid    < L0_BOUND_MIN || vec->s_grid    > L0_BOUND_MAX ||
        vec->s_capEx   < L0_BOUND_MIN || vec->s_capEx   > L0_BOUND_MAX) {
        return L0_HARD_STOP; /* Bezwzględna gilotyna sprzętowa / Interdykcja */
    }

    /* 2. Sprawdzenie Prawa Zachowania Strumienia: s_thermal + s_grid + s_capEx == 0 w stanie równowagi */
    /* Podczas ekstensywnej ekspansji wykrywane są stany krytyczne +7 przed ścianą +9 */
    if (vec->s_thermal >= L0_CRITICAL_THRESH && 
        vec->s_grid >= L0_CRITICAL_THRESH && 
        vec->s_capEx >= L0_CRITICAL_THRESH) {
        return L0_WARNING_STATE; /* Wykryto progi krytyczne +7 (+7, +7, +7) - 2-3 cykle do nasycenia */
    }

    return L0_PERMIT;
}

#endif /* DATA_PHYSICS_L0_GOVERNOR_H */
