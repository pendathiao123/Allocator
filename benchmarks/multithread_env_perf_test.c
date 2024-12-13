#define _POSIX_C_SOURCE 199309L  // Activer les fonctionnalités POSIX

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // Pour usleep()

#include "../include/my_allocator.h"

#define NUM_ITERATIONS 100
#define MAX_BLOCK_SIZE 4096
#define NUM_THREADS 2  // Nombre de threads à utiliser

// Prototype de la fonction thread_work
void* thread_work(void* arg);

// Structure pour configurer un cas de test
typedef struct {
    const char* test_name;
    size_t max_block_size;
    size_t step_size;
    int num_iterations;
} TestConfig;

typedef struct {
    void* (*alloc_func)(size_t);
    void (*free_func)(void*);
    size_t block_size;
    int iterations;
} ThreadData;

// Fonction de mesure du temps d'exécution avec clock_gettime()
double measure_time_with_threads(void* (*alloc_func)(size_t), void (*free_func)(void*), size_t block_size, int iterations, int num_threads) {
    struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        return -1;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    
    // Initialiser les données pour chaque thread
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].alloc_func = alloc_func;
        thread_data[i].free_func = free_func;
        thread_data[i].block_size = block_size;
        thread_data[i].iterations = iterations / num_threads;  // Partager les itérations entre les threads
        pthread_create(&threads[i], NULL, thread_work, (void*)&thread_data[i]);
    }

    // Attendre la fin de tous les threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        return -1;
    }

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed_time;
}

// Fonction exécutée par chaque thread
void* thread_work(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = 0; i < data->iterations; i++) {
        void* ptr = data->alloc_func(data->block_size);
        data->free_func(ptr);
    }
    return NULL;
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
        double my_alloc_time = measure_time_with_threads(my_malloc, my_free, block_size, config->num_iterations, NUM_THREADS);

        // Test pour malloc / free
        double sys_alloc_time = measure_time_with_threads(malloc, free, block_size, config->num_iterations, NUM_THREADS);

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
        {"test_1.csv", 1024, 8, 100},
        {"test_2.csv", 2048, 16, 50},
        {"test_3.csv", 4096, 32, 20}
    };

    size_t num_tests = sizeof(tests) / sizeof(tests[0]);

    // Lancer chaque test
    for (size_t i = 0; i < num_tests; i++) {
        run_test(&tests[i]);
    }

    return 0;
}
