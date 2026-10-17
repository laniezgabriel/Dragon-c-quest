================================================================================
PROJET SAÉ S1.01 : JEU D'AVENTURE TEXTUEL
Titre : Dragon-c quest
================================================================================

AUTEURS
-------
1. laniez gabriel et kwizera henry
Groupe : [TPE]

DESCRIPTION
-----------
"Dragon-C quest: The Forgotten Fortress" est un jeu d'aventure textuel (Interactive Fiction)
se déroulant dans un univers médiéval-fantastique post-apocalyptique. Le joueur incarne
un chevalier explorant un donjon volcanique. Le jeu propose une narration à embranchements,
des combats, et une gestion de ressources, le tout disponible en Français et en Anglais.

INSTRUCTIONS DE COMPILATION ET D'EXÉCUTION
------------------------------------------
Ce projet a été développé pour être portable (Windows, Linux, MacOS) en utilisant
uniquement les bibliothèques standard du C.

1. Pré-requis :
   Avoir le compilateur GCC installé.
   S'assurer que les fichiers suivants sont dans le même dossier :
   - main.c
   - histoire_fr.txt
   - story_en.txt

2. Compilation :
   Ouvrez un terminal dans le dossier du projet et tapez :
   gcc main.c -o jeu

3. Exécution :
   Sous Windows : jeu.exe
   Sous Linux/Mac : ./jeu

DÉFIS RÉALISÉS ET MÉTHODOLOGIE
------------------------------

1. Partie Obligatoire : Support Multi-langue
   - Problème : Afficher le texte dans la langue choisie par l'utilisateur sans dupliquer le code.
   - Méthode : Nous avons utilisé des fichiers textes externes (`.txt`) formatés avec un système d'ID
     (ex: "10:Texte"). Au lancement, le programme demande la langue, ouvre le fichier correspondant
     (fr ou en) et charge le contenu dans un tableau de structures (struct) en mémoire.
     L'affichage se fait ensuite en appelant l'ID de l'étape.

2. Défi Moyen : Système de Combat Textuel
   - Problème : Créer une interaction dynamique quand le joueur rencontre un monstre.
   - Méthode : Lorsqu'une étape est marquée comme "Combat", une boucle `while` s'enclenche.
     Nous utilisons des variables pour les PV du joueur et du monstre. À chaque tour,
     le programme calcule des dégâts (parfois aléatoires) et met à jour les statistiques
     jusqu'à ce que l'un des deux tombe à 0 PV.

3. Défi Moyen : Gestion de l'Inventaire et des Stats
   - Problème : L'équipement et les actions doivent influencer la suite du jeu.
   - Méthode : Nous avons ajouté des champs dans la structure du joueur (Force, Vie, Inventaire).
     Certains choix narratifs (ex: "Manger au buffet", "Ouvrir le coffre") incrémentent
     directement ces variables. Ces statistiques sont ensuite utilisées pour calculer
     les dégâts lors des combats.

4. Défi Facile : Événements Aléatoires (RNG)
   - Problème : Rendre certains coffres imprévisibles (Mimic vs Trésor).
   - Méthode : Utilisation de la fonction `rand()` pour générer un nombre. Selon le résultat,
     le jeu dirige le joueur vers l'ID du "Trésor" ou l'ID du "Piège/Combat".

5. Défi Facile : Gestion des erreurs de saisie
   - Problème : Éviter que le jeu plante si le joueur tape une mauvaise lettre.
   - Méthode : Mise en place d'une vérification des entrées (`scanf`) avec une boucle qui
     redemande la saisie tant que le choix n'est pas valide (1, 2 ou 3).

STRUCTURE DES FICHIERS
----------------------
- main.c          : Code source principal contenant la logique du jeu.
- histoire_fr.txt : Fichier de ressources contenant tous les textes en Français.
- story_en.txt    : Fichier de ressources contenant tous les textes en Anglais.
- README.txt      : Ce fichier (Documentation).

================================================================================