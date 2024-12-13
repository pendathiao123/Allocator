#define _POSIX_C_SOURCE 199309L  // Activer les fonctionnalités POSIX

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // Pour usleep()

#include "../include/my_allocator.h"

#define NUM_ITERATIONS 1000000
#define MAX_BLOCK_SIZE 4096

// Structure pour configurer un cas de test
typedef struct {
    const char* test_name;
    size_t max_block_size;
    size_t step_size;
    int num_iterations;
} TestConfig;

// Fonction de mesure du temps d'exécution avec clock_gettime()
double measure_time(void* (*alloc_func)(size_t), void (*free_func)(void*), size_t block_size, int iterations) {
    struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        return -1;
    }

    for (int i = 0; i < iterations; i++) {
        void* ptr = alloc_func(block_size);
        free_func(ptr);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        return -1;
    }

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed_time;
}

// Fonction de test pour une configuration spécifique
void run_test(const TestConfig* config) {
    FILE* output = fopen(config->test_name, "w");
    if (!output) {
        perror("Failed to open file");
        return;
    }

    fprintf(output, "Block Size,My Allocator,System Allocator\n");

    for (size_t block_size = 8; block_size <= config->max_block_size; block_size += config->step_size) {
        // Test pour my_malloc / my_free
        double my_alloc_time = measure_time(my_malloc, my_free, block_size, config->num_iterations);

        // Test pour malloc / free
        double sys_alloc_time = measure_time(malloc, free, block_size, config->num_iterations);

        // Écrire les résultats dans le fichier CSV
        fprintf(output, "%zu,%.6f,%.6f\n", block_size, my_alloc_time, sys_alloc_time);
        printf("Block Size: %zu, My Allocator: %.6f s, System Allocator: %.6f s\n",
               block_size, my_alloc_time, sys_alloc_time);
    }

    fclose(output);
    printf("Performance results written to '%s'\n", config->test_name);
}

int main() {
    // Définir plusieurs configurations de tests
    TestConfig tests[] = {
        {"test_1.csv", 1024, 8, 100000},
        {"test_2.csv", 2048, 16, 50000},
        {"test_3.csv", 4096, 32, 20000}
    };

    size_t num_tests = sizeof(tests) / sizeof(tests[0]);

    // Lancer chaque test
    for (size_t i = 0; i < num_tests; i++) {
        run_test(&tests[i]);
    }

    return 0;
}
