import matplotlib.pyplot as plt
import csv

# Initialisation des listes pour les données
block_sizes = []
time_my_allocator = []
time_malloc_free = []

# Lire les données depuis le fichier CSV (supposé généré par le programme C)
try:
    with open('performance_data.csv', 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Ignorer l'en-tête
        for row in reader:
            block_sizes.append(int(row[0]))  # Taille du bloc en bytes
            time_my_allocator.append(float(row[1]))  # Temps pour my_malloc/my_free
            time_malloc_free.append(float(row[2]))  # Temps pour malloc/free
except FileNotFoundError:
    print("Erreur : Le fichier 'performance_data.csv' n'a pas été trouvé.")
    exit(1)
except Exception as e:
    print(f"Erreur lors de la lecture du fichier CSV : {e}")
    exit(1)

# Nombre d'allocations (constant dans cet exemple)
num_allocations = 1000000

# Tracer les performances
plt.figure(figsize=(10, 6))

# Graphique pour my_malloc/my_free
plt.plot(block_sizes, time_my_allocator, label='my_malloc/my_free', marker='o', color='blue')

# Graphique pour malloc/free
plt.plot(block_sizes, time_malloc_free, label='malloc/free', marker='x', color='red')

# Ajouter des annotations pour chaque point
for i, size in enumerate(block_sizes):
    plt.annotate(f'{num_allocations}', 
                 (block_sizes[i], time_my_allocator[i]), 
                 textcoords="offset points", xytext=(0,10), ha='center', color='blue', fontsize=8)
    plt.annotate(f'{num_allocations}', 
                 (block_sizes[i], time_malloc_free[i]), 
                 textcoords="offset points", xytext=(0,10), ha='center', color='red', fontsize=8)

# Configuration des axes
plt.xscale('log')  # Utilisation d'une échelle logarithmique sur l'axe des X
plt.xlabel('Taille du bloc (bytes)')  # Axe des abscisses (taille du bloc)
plt.ylabel('Temps écoulé (secondes)')  # Axe des ordonnées (temps écoulé)
plt.title(f'Comparaison des performances entre my_malloc/my_free et malloc/free \n'
          f'Nombre d\'allocations: {num_allocations}')  # Titre du graphique
plt.legend()  # Légende pour distinguer les deux courbes
plt.grid(True, which="both", ls="--")  # Ajouter une grille pour améliorer la lisibilité
plt.tight_layout()  # Ajuster automatiquement les marges

# Sauvegarder l'image du graphique dans un fichier PNG
plt.savefig('perf.png')
