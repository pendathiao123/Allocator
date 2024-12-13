# Variables
CC = gcc
# Valeur par défaut de CFLAGS
CFLAGS_DEFAULT = -Wall -g
# Compile avec O2
CFLAGS_O2 = -Wall -g -O2 -march=native
# Compile avec O3
CFLAGS_O3 = -Wall -g -O3 -march=native

# Utilisation d'un argument pour choisir les options de compilation
ifeq ($(O),O2)
    CFLAGS = $(CFLAGS_O2)
else ifeq ($(O),O3)
    CFLAGS = $(CFLAGS_O3)
else
    CFLAGS = $(CFLAGS_DEFAULT)
endif

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
BENCHMARK_DIR = benchmarks
TESTS_DIR = tests_validation

# Fichiers sources et headers
COMMON_SRCS = $(SRC_DIR)/my_allocator.c
COMMON_HDRS = $(INCLUDE_DIR)/my_allocator.h

# Cibles pour les tests unitaires
TESTS_TARGET = $(BUILD_DIR)/my_allocator_tests
TESTS_SRCS = $(COMMON_SRCS) $(TESTS_DIR)/tests.c
TESTS_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(TESTS_SRCS))

# Cibles pour les tests de performance
PERFORMANCE_TARGET = $(BUILD_DIR)/performance_test
PERFORMANCE_SRCS = $(COMMON_SRCS) $(BENCHMARK_DIR)/performance_test.c
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
	@echo "  make all O=O2 - Compile avec l'option -O2"
	@echo "  make all O=O3 - Compile avec l'option -O3"
