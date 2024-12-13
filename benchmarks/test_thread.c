#define _POSIX_C_SOURCE 199309L  // Activer les fonctionnalités POSIX

#include "../include/my_allocator.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM_THREADS 2
// La définition de BLOCK_SIZE est déjà dans my_allocator.h, donc tu peux supprimer cette ligne.

// Structure pour passer des données à chaque thread
typedef struct {
    void* (*alloc_func)(size_t);
    void (*free_func)(void*);
    long thread_id;
} ThreadData;

// Déclaration de la fonction thread_func avant son utilisation
void* thread_func(void* arg);

// Fonction de mesure du temps d'exécution
double measure_time(void* (*alloc_func)(size_t), void (*free_func)(void*), size_t block_size, int num_threads) {
    struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        return -1;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Initialiser les données pour chaque thread
    for (long i = 0; i < num_threads; i++) {
        thread_data[i].alloc_func = alloc_func;
        thread_data[i].free_func = free_func;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], NULL, thread_func, (void*)&thread_data[i]);
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
void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    void* ptr = data->alloc_func(BLOCK_SIZE); 
    if (ptr) {
        printf("Thread %ld allocated memory at %p\n", data->thread_id, ptr);
        data->free_func(ptr);
    }
    return NULL;
}

int main() {
    // Mesurer le temps pour my_malloc / my_free
    double my_alloc_time = measure_time(my_malloc, my_free, BLOCK_SIZE, NUM_THREADS);
    printf("Time taken using my_malloc/my_free: %.6f seconds\n", my_alloc_time);

    // Mesurer le temps pour malloc / free
    double sys_alloc_time = measure_time(malloc, free, BLOCK_SIZE, NUM_THREADS);
    printf("Time taken using malloc/free: %.6f seconds\n", sys_alloc_time);

    return 0;
}
