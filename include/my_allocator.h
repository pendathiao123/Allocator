/*
#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <stddef.h>

// Déclaration des fonctions
void* my_malloc(size_t size);
void my_free(void* ptr);

#endif // ALLOCATOR_H
*/

#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <stddef.h>

// Déclaration des fonctions
void* my_malloc(size_t size);
void my_free(void* ptr);
void coalesce_blocks(); // Fonction de coalescence des blocs libres
#endif // MY_ALLOCATOR_H
