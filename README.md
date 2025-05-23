# Net 

**Net** est un jeu de logique développé en **C** dans le cadre d’un **projet universitaire réalisé en équipe de 3 étudiants**. L’objectif est de faire pivoter des pièces sur une grille 5x5 pour former un **réseau connecté**. Contrairement à la règle d’origine, cette version **accepte les boucles** et **autorise des cases vides**.

## Contenu du projet

Le projet repose sur une seule bibliothèque nommée `libgame.a`, qui propose une architecture modulaire organisée comme suit :

- `game.h` — Fonctions de base du jeu  
- `game_aux.h` — Fonctions auxiliaires  
- `game_ext.h` — Fonctions avancées (extensions)  

Ces interfaces utilisent un **type opaque** (`game`) manipulé via des pointeurs (`game*` ou `cgame` pour les constantes).

## Principes techniques

- Types opaques & encapsulation  
- Gestion stricte des erreurs avec `assert()`  
- Utilisation de pointeurs `const` pour la sûreté  
- Architecture modulaire

## Fonctionnalités proposées

Le dépôt fournit plusieurs programmes s’appuyant sur la bibliothèque :

- `game_text` — Version textuelle du jeu, jouable dans le terminal  
- `game_test` — Programme de tests unitaires pour valider la bibliothèque  
- `game_solve` — Solveur automatique qui génère une solution valide  
- `game_sdl` — Interface graphique basée sur la bibliothèque SDL  
- `web_version` — Une **version web intégrée** du jeu (interface HTML/JS fournie)  

## Règles du jeu

Inspiré de la version originale de Simon Tatham :

> Faites tourner les pièces pour former un seul réseau connecté, sans boucles.

### Adaptations apportées

- Les boucles sont **autorisées**
- Des **cases vides** peuvent apparaître dans la grille

Chaque pièce (segment, coin, extrémité, ou T) contient un ou plusieurs **demi-bords** connectables. L’alignement des demi-bords entre deux cases adjacentes permet de former des arêtes dans le graphe.

### Objectif

Unifier tous les éléments connectés en un seul **graphe connexe**. Le jeu est considéré comme résolu quand toutes les connexions sont valides et forment un réseau complet.

## Références

- [Wikipedia — Net (puzzle)](https://en.wikipedia.org/wiki/Net)
