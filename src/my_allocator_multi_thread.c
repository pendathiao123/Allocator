#include "../include/my_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>  // Pour le multithreading

// Listes libres pour chaque classe de tailles
static Block* free_lists[NUM_CLASSES] = {NULL};

// Table des tailles maximales par classe pour chaque classe
static const size_t max_class_sizes[NUM_CLASSES] = {
    (1UL << 0), (1UL << 1), (1UL << 2), (1UL << 3), (1UL << 4),
    (1UL << 5), (1UL << 6), (1UL << 7), (1UL << 8), (1UL << 9)};

// Compteur pour le nombre de pointeurs actifs
static size_t active_pointers = 0;

// Mutex global pour protéger l'accès aux ressources partagées
static pthread_mutex_t allocator_mutex = PTHREAD_MUTEX_INITIALIZER;

// Fonction pour trouver la classe de taille correspondante
int get_class(size_t size) {
    int class = 0;
    while (class < NUM_CLASSES && max_class_sizes[class] < size) {
        class++;
    }
    return class;
}

// Fonction pour aligner une taille à la puissance de 2 la plus proche
size_t align_to_power_of_two(size_t size) {
    size_t aligned_size = (size + 7) & ~7;  // Aligner sur 8 octets
    return aligned_size;
}

// Recherche ou allocation d'un bloc mémoire
void* my_malloc(size_t size) {
    if (size == 0) return NULL;

    size_t aligned_size = align_to_power_of_two(size + BLOCK_SIZE); // On ajoute BLOCK_SIZE pour l'en-tête
    int class = get_class(aligned_size);

    if (class >= NUM_CLASSES) {
        fprintf(stderr, "Invalid class size %zu\n", aligned_size);
        return NULL;  // Retourner NULL si la classe est invalide
    }

    // Verrouiller l'accès avant de manipuler les listes libres
    pthread_mutex_lock(&allocator_mutex);

    // Vérifier si un bloc est disponible dans la liste libre
    Block* block = free_lists[class];
    if (block) {
        free_lists[class] = block->next;
        block->free = 0;
        active_pointers++; // Incrémenter le compteur
        printf("Allocated block at %p of size %zu\n", block, aligned_size);
        pthread_mutex_unlock(&allocator_mutex); // Déverrouiller après l'allocation
        return (void*)(block + 1);
    }

    // Aucun bloc disponible, allocation avec mmap
    block = mmap(NULL, aligned_size + sizeof(Block), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED) {
        perror("mmap");
        pthread_mutex_unlock(&allocator_mutex); // Déverrouiller en cas d'erreur
        return NULL;
    }
    block = (Block*)((char*)block + sizeof(Block));  // Décale le pointeur pour ne pas écraser les métadonnées
    block->size = aligned_size;
    block->next = NULL;
    block->free = 0;
    active_pointers++; // Incrémenter le compteur
    printf("Allocated block at %p of size %zu with mmap\n", block, aligned_size);

    pthread_mutex_unlock(&allocator_mutex); // Déverrouiller après l'allocation
    return (void*)(block + 1);
}

// Libération et recyclage du bloc mémoire
void my_free(void* ptr) {
    if (!ptr) return;

    Block* block = (Block*)ptr - 1; // Adresse du bloc mémoire avant le pointeur

    // Vérification pour éviter les erreurs de double libération
    if (block->free) {
        fprintf(stderr, "Double free detected at block %p, size %zu\n", block, block->size);
        return;
    }

    // Verrouiller l'accès avant de manipuler les listes libres
    pthread_mutex_lock(&allocator_mutex);

    // Marquer comme libre
    block->free = 1;
    block->next = free_lists[get_class(block->size)];
    free_lists[get_class(block->size)] = block;

    // Fusionner les blocs adjacents libres (si nécessaire)
    coalesce_free_blocks(get_class(block->size));

    active_pointers--; // Décrémenter le compteur des blocs actifs
    printf("Freed block at %p of size %zu\n", block, block->size);

    pthread_mutex_unlock(&allocator_mutex); // Déverrouiller après la libération
}

// Fusion des blocs libres adjacents dans une classe spécifique
void coalesce_free_blocks(int class) {
    Block* current = free_lists[class];
    while (current && current->next) {
        // Vérification de la validité des pointeurs avant l'accès
        if ((char*)current + current->size == (char*)current->next) {
            // Fusion des blocs si adjacents et libres
            if (current->free && current->next->free) {
                current->size += current->next->size; // Fusionner les blocs
                current->next = current->next->next;  // Supprimer le bloc fusionné
                printf("Coalesced blocks at %p and %p\n", current, current->next);
            } else {
                current = current->next;
            }
        } else {
            break; // Les blocs ne sont pas adjacents, on arrête la fusion
        }
    }
}

// Fonction pour afficher le nombre de pointeurs actifs
void print_active_pointers() {
    printf("Active pointers: %zu\n", active_pointers);
}
