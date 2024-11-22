# Hillclimber Personnalisé pour Problème NRPB 2

## Résumé

Ce projet implémente un **Hillclimber Personnalisé** pour résoudre des problèmes d'optimisation, notamment le problème **NRPB 2**, caractérisé par la présence de plateaux dans l'espace de recherche. L'algorithme utilise des mécanismes avancés pour détecter et éviter ces plateaux, permettant une exploration plus efficace.

---

## Fonctionnalités Principales

### Détection des Plateaux
L'algorithme détecte les plateaux lorsque les gains en coût (\(\Delta C\)) sont inférieurs à un seuil (\(\epsilon = 0.05\)) pendant plusieurs itérations consécutives (\(T_{\text{stagnant}}\)).

### Ajustement Dynamique du Pas
Le rayon de recherche (\(\Delta\)) est ajusté dynamiquement pour échapper aux plateaux. La valeur de \(\Delta\) augmente en fonction du nombre d'itérations stagnantes et est plafonnée à une limite maximale (\(\Delta_{\text{max}} = 5.0\)).

### Sauts Globaux Aléatoires
Lorsque l'algorithme stagne pendant \(T_{\text{plateau}}\) itérations, il effectue un **saut global** dans l'espace de recherche pour explorer de nouvelles régions.

### Exploitation de l’Historique des Coûts
Les sauts globaux sont biaisés vers des zones prometteuses en fonction de l'historique des coûts précédemment observés.

### Critère d’Arrêt
Si les ajustements dynamiques et les sauts globaux ne parviennent pas à sortir des plateaux, l'algorithme stoppe prématurément la recherche.

---

## Paramètres Clés

| **Paramètre**        | **Valeur**    |
|-----------------------|---------------|
| Seuil de stagnation (\(\epsilon\)) | 0.05          |
| Pas initial (\(\Delta_{\text{base}}\)) | 0.00025       |
| Facteur multiplicatif (\(\alpha\))  | 1.5           |
| Pas maximal (\(\Delta_{\text{max}}\)) | 5.0           |
| Itérations plateau (\(T_{\text{plateau}}\)) | 100           |
| Tolérance (\(LIMIT_{\text{TOLERANCE}}\)) | 0.0001        |

---

## Installation et Exécution

### Prérequis
- **C++17** ou version ultérieure.
- Bibliothèques nécessaires pour l'interaction avec **Scilab** (si simulation intégrée).

### Compilation
Pour compiler le programme, utilisez la commande suivante :
```bash
g++ hillclimber.cpp scilabcomm.cpp -o hillclimber -std=c++17


