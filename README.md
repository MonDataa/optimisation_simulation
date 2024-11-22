# Hillclimber Personnalisé pour Problème NRPB 2

## Résumé

Ce projet implémente un **Hillclimber Personnalisé** pour résoudre des problèmes d'optimisation, notamment le problème **NRPB 2**, caractérisé par la présence de plateaux dans l'espace de recherche. L'algorithme utilise des mécanismes avancés pour détecter et éviter ces plateaux, permettant une exploration plus efficace.

---

## Fonctionnalités Principales

### Détection des Plateaux
L'algorithme détecte les plateaux lorsque les gains en coût (\( \Delta C \)) sont inférieurs à un seuil (\( \epsilon = 0.05 \)) pendant plusieurs itérations consécutives (\( T_{\text{stagnant}} \)) :

\[
\Delta C_i = \left| C(s_{i+1}) - C(s_i) \right| < \epsilon, \quad \forall i \in [1, T_{\text{stagnant}}]
\]

### Ajustement Dynamique du Pas
Pour échapper aux plateaux, le rayon de recherche (\( \Delta \)) est ajusté dynamiquement en fonction du nombre d'itérations stagnantes (\( n \)) :

\[
\Delta_{\text{adaptive}} = \min(\Delta_{\text{base}} \cdot \alpha^n, \Delta_{\text{max}})
\]

- \( \Delta_{\text{base}} \) : Pas initial (par ex., \( 0.00025 \)).
- \( \alpha \) : Facteur multiplicatif (\( \alpha = 1.5 \)).
- \( n \) : Nombre d'itérations consécutives sur un plateau.
- \( \Delta_{\text{max}} \) : Pas maximal (\( 5.0 \)).

### Sauts Globaux Aléatoires
Lorsque l'algorithme stagne pendant \( T_{\text{plateau}} \) itérations, il effectue un **saut global** dans l'espace de recherche pour explorer de nouvelles régions :

\[
s_{\text{new}} = X_{\text{min}} + \beta \cdot (X_{\text{max}} - X_{\text{min}})
\]

- \( X_{\text{min}} \), \( X_{\text{max}} \) : Limites de l'espace de recherche.
- \( \beta \sim \mathcal{U}(0, 1) \) : Variable aléatoire uniforme.

### Exploitation de l’Historique des Coûts
Les sauts globaux sont biaisés vers des zones prometteuses en fonction de l'historique des coûts précédemment observés :

\[
s_{\text{new}} = X_{\text{min}} + \frac{C(s_{\text{current}}) - C_{\text{min}}}{C_{\text{max}} - C_{\text{min}}} \cdot (X_{\text{max}} - X_{\text{min}})
\]

### Critère d’Arrêt
Si les ajustements dynamiques et les sauts globaux ne parviennent pas à sortir des plateaux, l'algorithme stoppe prématurément la recherche.

---

## Paramètres Clés

| **Paramètre**                       | **Valeur**    |
|-------------------------------------|---------------|
| Seuil de stagnation (\( \epsilon \))| 0.05          |
| Pas initial (\( \Delta_{\text{base}} \)) | 0.00025       |
| Facteur multiplicatif (\( \alpha \))| 1.5           |
| Pas maximal (\( \Delta_{\text{max}} \)) | 5.0           |
| Itérations plateau (\( T_{\text{plateau}} \)) | 100           |
| Tolérance (\( LIMIT_{\text{TOLERANCE}} \)) | 0.0001        |

---
