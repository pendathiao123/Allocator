#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <stddef.h>

// Définition de la structure Block
typedef struct Block {
    size_t size;       // Taille du bloc
    struct Block* next; // Pointeur vers le bloc suivant dans la liste libre
    int free;          // Indicateur si le bloc est libre ou non
} Block;

#define PAGE_SIZE 4096
#define BLOCK_SIZE sizeof(Block)
#define NUM_CLASSES 10 // Nombre de classes pour les tailles de blocs (puissances de 2)

// Fonctions de l'allocateur
void* my_malloc(size_t size);
void my_free(void* ptr);
void coalesce_free_blocks(int class);
void print_active_pointers();

#endif // MY_ALLOCATOR_H
