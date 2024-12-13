#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../include/my_allocator.h"

void run_tests() {
    printf("Running tests...\n");

    // Test 1 : Allocation simple
    size_t size1 = 1024;
    void* ptr1 = my_malloc(size1);
    assert(ptr1 != NULL);
    printf("Test 1 passed: Allocation simple\n");

    // Test 2 : Écriture et lecture
    strcpy((char*)ptr1, "Test data");
    assert(strcmp((char*)ptr1, "Test data") == 0);
    printf("Test 2 passed: Écriture et lecture\n");

    // Test 3 : Libération
    my_free(ptr1);  // Pas besoin de taille ici
    printf("Test 3 passed: Libération\n");

    // Test 4 : Allocation nulle
    void* ptr2 = my_malloc(0);
    assert(ptr2 == NULL);
    printf("Test 4 passed: Allocation nulle\n");

    // Test 5 : Libération sur NULL
    my_free(NULL);  // Pas de vérification du retour
    printf("Test 5 passed: Libération sur NULL\n");

    printf("All tests passed!\n");
}

int main() {
    run_tests();
    return 0;
}
