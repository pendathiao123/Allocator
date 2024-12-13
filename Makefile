# Variables
CC = gcc
#Compile sans optimisation
#CFLAGS = -Wall -g 
#Compile avec O2
#CFLAGS = -Wall -g -O2 -march=native
#Vompile with O3
CFLAGS = -Wall -g -O3 -march=native
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

# Fichiers sources et headers
COMMON_SRCS = $(SRC_DIR)/my_allocator.c
COMMON_HDRS = $(INCLUDE_DIR)/my_allocator.h

# Cibles pour les tests unitaires
TESTS_TARGET = $(BUILD_DIR)/my_allocator_tests
TESTS_SRCS = $(COMMON_SRCS) $(SRC_DIR)/tests.c
TESTS_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(TESTS_SRCS))

# Cibles pour les tests de performance
PERFORMANCE_TARGET = $(BUILD_DIR)/performance_test
PERFORMANCE_SRCS = $(COMMON_SRCS) $(SRC_DIR)/performance_test.c
PERFORMANCE_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(PERFORMANCE_SRCS))

# Règle par défaut : compiler et lier les tests unitaires et de performance
all: $(TESTS_TARGET) $(PERFORMANCE_TARGET)

# Compilation des tests unitaires
$(TESTS_TARGET): $(TESTS_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TESTS_OBJS) -o $(TESTS_TARGET)

# Compilation des tests de performance
$(PERFORMANCE_TARGET): $(PERFORMANCE_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PERFORMANCE_OBJS) -o $(PERFORMANCE_TARGET)

# Compilation des fichiers objets pour les tests unitaires et de performance
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(COMMON_HDRS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Création du dossier build si nécessaire
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)

# Aide pour l'utilisateur
help:
	@echo "Cibles disponibles :"
	@echo "  all       - Compile tous les exécutables"
	@echo "  clean     - Supprime les fichiers générés"
	@echo "  help      - Affiche cette aide"
