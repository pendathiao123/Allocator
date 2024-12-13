import matplotlib.pyplot as plt
import pandas as pd


def plot_performance(ax, file_name, label):
   # Charger les résultats depuis le fichier CSV
   data = pd.read_csv(file_name)


   # Extraire les colonnes
   block_sizes = data["Block Size"]
   my_alloc_times = data["My Allocator"]
   sys_alloc_times = data["System Allocator"]
   # Création du graphique pour le scénario courant
   ax.plot(block_sizes, my_alloc_times, label="My Allocator", marker="o")
   ax.plot(block_sizes, sys_alloc_times, label="System Allocator", marker="s")
   ax.set_title(label)
   ax.set_xlabel("Block Size (bytes)")
   ax.set_ylabel("Time (seconds)")
   ax.legend()
   ax.grid(True, linestyle="--", alpha=0.7)


# Fichiers de test et labels correspondants
test_files = ["test_1.csv", "test_2.csv", "test_3.csv"]
test_labels = ["Max 1024 octets,pas de 8 octets, 100 000 iter",
              "Max 2048 octets,pas de 16 octets, 50 000 iter",
              "Max 4096 octets, pas de 32 octets, 20 000 iter"]


# Création des sous-graphiques
fig, axes = plt.subplots(nrows=1, ncols=len(test_files), figsize=(15, 5), sharey=True)


for ax, file, label in zip(axes, test_files, test_labels):
   plot_performance(ax, file, label)


# Ajustement des espaces entre les graphiques
fig.suptitle("Performance Comparison: My Allocator vs System Allocator")
fig.tight_layout(rect=[0, 0.03, 1, 0.95])


# Sauvegarder et afficher le graphique
plt.savefig("performance_comparison_multiple_tests.png", dpi=300)
plt.show()
