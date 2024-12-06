#include "my_allocator.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stddef.h>
#include <assert.h>

#define NUM_CLASSES 16       // Nombre de classes de tailles
#define ALIGNMENT 16         // Alignement des blocs de mémoire

// Structure représentant un bloc de mémoire
typedef struct Block {
    size_t size;           // Taille du bloc (sans compter les métadonnées)
    struct Block* next;    // Pointeur vers le bloc suivant
} Block;

// Mutex pour protéger l'accès aux listes de blocs libres
static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
static Block* free_lists[NUM_CLASSES] = {NULL};  // Liste des blocs libres par classe de taille

// Fonction pour aligner la taille à un multiple de ALIGNMENT
size_t align_size(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

// Fonction pour déterminer la classe de taille en fonction de la taille du bloc
size_t get_class(size_t size) {
    size_t class = 0;
    while ((1 << class) < size) {
        class++;
    }
    return class < NUM_CLASSES ? class : NUM_CLASSES - 1;
}

// Fonction d'allocation personnalisée
void* my_malloc(size_t size) {
    if (size == 0) return NULL;

    // Alignement de la taille et ajout de l'espace pour les métadonnées
    size = align_size(size + sizeof(Block));
    size_t class = get_class(size);

    pthread_mutex_lock(&global_lock);

    // Recherche d'un bloc libre dans la liste de la classe correspondante
    Block* block = free_lists[class];
    if (block) {
        free_lists[class] = block->next;
        pthread_mutex_unlock(&global_lock);
        return (void*)((char*)block + sizeof(Block));  // Retourner l'adresse après les métadonnées
    }

    pthread_mutex_unlock(&global_lock);

    // Si aucun bloc libre, allouer via mmap
    block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (block == MAP_FAILED) {
        return NULL;  // Gestion des erreurs d'allocation
    }
    block->size = size;
    return (void*)((char*)block + sizeof(Block));  // Retourner l'adresse après les métadonnées
}

// Fonction de libération personnalisée
void my_free(void* ptr) {
    if (ptr == NULL) return;

    Block* block = (Block*)((char*)ptr - sizeof(Block));  // Pointeur vers le bloc
    size_t class = get_class(block->size);

    pthread_mutex_lock(&global_lock);

    // Ajout du bloc à la liste des blocs libres pour la classe correspondante
    block->next = free_lists[class];
    free_lists[class] = block;

    pthread_mutex_unlock(&global_lock);

    // Essayer de coalescer les blocs adjacents
    coalesce_blocks();
}

// Fonction pour fusionner les blocs adjacents (coalescence)
void coalesce_blocks() {
    pthread_mutex_lock(&global_lock);

    // Pour chaque classe de taille
    for (size_t class = 0; class < NUM_CLASSES; class++) {
        Block* current = free_lists[class];
        while (current && current->next) {
            // Vérifier si les blocs sont adjacents en mémoire
            if ((char*)current + current->size == (char*)current->next) {
                // Fusionner les blocs adjacents
                current->size += current->next->size;
                current->next = current->next->next;  // Retirer le bloc fusionné
            } else {
                current = current->next;  // Passer au bloc suivant
            }
        }
    }

    pthread_mutex_unlock(&global_lock);
}
