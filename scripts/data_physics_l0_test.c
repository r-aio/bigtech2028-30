/**
 * @file data_physics_l0_test.c
 * @brief Hardware-in-the-Loop (HIL) Driver for Data Physics & L0-ISM Infrastructure Simulation
 */

#include <stdio.h>
#include <stdlib.h>
#include "data_physics_l0_governor.h"

int main(void) {
    printf("=== Data Physics & L0-ISM Hardware Interdiction Driver Simulation ===\n\n");

    data_physics_vector_t test_scenarios[] = {
        { .s_thermal = 2, .s_grid = 2, .s_capEx = 1 },   /* 2020-2022: Nominal Safety Margins */
        { .s_thermal = 5, .s_grid = 4, .s_capEx = 5 },   /* 2023-2024: Accelerated Expansion */
        { .s_thermal = 7, .s_grid = 7, .s_capEx = 7 },   /* 2025-2027: Critical Threshold (+7, +7, +7) */
        { .s_thermal = 9, .s_grid = 7, .s_capEx = 8 },   /* 2028-2030: Thermal Saturation Boundary (+9) */
        { .s_thermal = 9, .s_grid = 9, .s_capEx = 9 }    /* 2028-2030: Absolute L0 Saturation Collision */
    };

    const char* labels[] = {
        "Phase I (2020-2022) - Nominal Margins",
        "Phase II (2023-2024) - High Load",
        "Phase III (2025-2027) - Critical State (+7, +7, +7)",
        "Phase IV-A (2028-2029) - Thermal Boundary Breach",
        "Phase IV-B (2028-2030) - Full L0 Matrix Saturation (+9)"
    };

    size_t total_tests = sizeof(test_scenarios) / sizeof(test_scenarios[0]);
    int errors = 0;

    for (size_t i = 0; i < total_tests; i++) {
        int16_t result = verify_data_physics_state(&test_scenarios[i]);
        printf("[%s]\n Vector: (s_thermal=%d, s_grid=%d, s_capEx=%d) -> Output Code: %d ",
               labels[i], test_scenarios[i].s_thermal, test_scenarios[i].s_grid, test_scenarios[i].s_capEx, result);

        if (result == L0_PERMIT) {
            printf("[PERMIT: Infrastructure Operational]\n\n");
        } else if (result == L0_WARNING_STATE) {
            printf("[WARNING: Critical Threshold (+7) Reached - 2 to 3 Cycles to Wall]\n\n");
        } else if (result == L0_HARD_STOP) {
            printf("[HARD_STOP: Bekenstein Boundary Saturation (+9) - Interdiction Executed]\n\n");
        } else {
            printf("[UNKNOWN ERROR]\n\n");
            errors++;
        }
    }

    if (errors == 0) {
        printf(">>> DATA PHYSICS HARDWARE GOVERNOR SIMULATION PASSED SUCCESSFULLY. <<<\n");
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
