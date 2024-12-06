#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include "my_allocator.h"

// Fonction pour mesurer le temps d'exécution des allocations et désallocations
double measure_time(void* (*alloc_func)(size_t), void (*free_func)(void*), size_t block_size, int num_allocations) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    void* ptr;
    for (int i = 0; i < num_allocations; i++) {
        ptr = alloc_func(block_size); 
        assert(ptr != NULL);
        free_func(ptr); // Libère la mémoire après allocation.
    }

    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    return elapsed_time;
}

int main() {
    size_t block_size;
    int num_allocations = 100000; // Nombre d'allocations à tester
    FILE* file = fopen("performance_data.csv", "w");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    // En-tête CSV avec une colonne pour le nombre d'allocations
    fprintf(file, "Taille du bloc (bytes),Temps my_malloc/my_free (secondes),Temps malloc/free (secondes),Nombre d'allocations\n");

    // Mesure de la performance pour différentes tailles de blocs
    for (block_size = 64; block_size <= 8*8*1024 * 1024; block_size *= 2) {
        double time_my_allocator = measure_time(my_malloc, my_free, block_size, num_allocations);
        double time_malloc_free = measure_time(malloc, free, block_size, num_allocations);

        // Écriture des résultats dans le fichier CSV, y compris le nombre d'allocations
        fprintf(file, "%zu,%.6f,%.6f,%d\n", block_size, time_my_allocator, time_malloc_free, num_allocations);
    }

    fclose(file);
    printf("Données sauvegardées dans performance_data.csv\n");
    return 0;
}
