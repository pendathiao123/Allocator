
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

typedef struct Block {
    size_t size;
    struct Block* next; // Pointeur vers le bloc suivant
    int free;
    void* ptr;  // Pointeur vers le bloc mémoire alloué
} Block;

static Block* free_list = NULL; // Liste des blocs libres

void* my_malloc(size_t size) {
    if (size == 0) return NULL;

    // Chercher un bloc libre qui peut contenir la taille demandée
    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            current->free = 0;
            return current->ptr;
        }
        current = current->next;
    }

    // Aucun bloc libre trouvé, allouer un nouveau bloc
    Block* new_block = (Block*)malloc(sizeof(Block) + size);
    if (!new_block) return NULL;

    new_block->size = size;
    new_block->free = 0;
    new_block->ptr = (void*)(new_block + 1);
    new_block->next = free_list;
    free_list = new_block;

    return new_block->ptr;
}

void coalesce_free_blocks() {
    Block* current = free_list;
    while (current != NULL && current->next != NULL) {
        if (current->free && current->next->free) {
            // Fusionner les blocs libres adjacents
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void my_free(void* ptr) {
    if (!ptr) return;

    // Trouver le bloc associé à ptr
    Block* current = free_list;
    while (current != NULL) {
        if (current->ptr == ptr) {
            current->free = 1;
            coalesce_free_blocks(); // Fusionner les blocs libres adjacents
            return;
        }
        current = current->next;
    }

    // Si le bloc n'a pas été trouvé, c'est une erreur
    fprintf(stderr, "Tentative de libération d'un bloc non alloué\n");
}

void print_allocations() {
    Block* current = free_list;
    while (current != NULL) {
        printf("Block at %p, size: %zu, free: %d\n", current->ptr, current->size, current->free);
        current = current->next;
    }
}
