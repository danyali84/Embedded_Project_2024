INF1900 - Projet initial de système embarqué

Auteurs: Antoine Beaunoyer, Samer Chebair, Myriam Turki et Danya Li
Numéro de groupe: 1923
Nom du robot: MASD

Notre dossier projet ce compose de deux sous-sections: exec (contenant l'executable) et lib (contenant la librairie).

Concernant la sous-section exec:
Elle est divisé en deux parties: app1 et app2.

app1 contient le fichier robot1 :
    - le fichier robot1 fait appel a des fonctions de la librairie pour effectuer les tâches.
    - la fonction findingCenterBox utilise un capteur de distance pour trouver le centre d'une boîte. 
    - Afin d'écrire le template en mémoire nous utilisons les fonctions calculate adress, writeVerticalLine and writeHorizantalline
    
    -  Afin d'afficher le robot, nous avons la fonction drawingrobot1:
            -  Si la direction est vraie, le robot est orienté vers la droite. Dans ce cas, la fonction écrit des caractères spécifiques 
            dans la mémoire aux positions calculées pour représenter le robot dans sa configuration orientée vers la droite.
            - Si la direction est fausse, le robot est orienté vers la gauche. Dans ce cas, la même chose mais avec des positions différentes.
   
   - Pour rajouter les segments a la carte nous avons la fonction addLineToMap:
            - Si la position est inférieure ou égale à 49, la ligne est ajoutée dans la partie supérieure de la carte. 
            - Si la position est supérieure à 49, la ligne est ajoutée dans la partie inférieure de la carte.
            (La position est ajustée pour correspondre à la disposition des lignes dans la mémoire.)

    - La fonction displayImage affiche une image représentée dans la mémoire sur le PC.
            - Parcourant la mémoire, elle lit chaque octet de données stocké et convertit son code en caractère Unicode correspondant, 
            puis l'envoie via l'interface UART pour l'affichage.
    
    - La fonction calculateDistance prend en paramètre une valeur data ette fonction est utilisée pour convertir une mesure 
    brute en une valeur de distance correspondant à une catégorie prédéfinie.

    - Le main est divisé en états qui représentent les différentes tâches du robot 1.

app2 contient le fichier robot2:

    - La fonction initialRoutine effectue une séquence de clignotement des LEDs en vert et en rouge pour indiquer 
    le démarrage du système.

    - La fonction passShortSides guide le robot pour passer les côtés courts de la zone à cartographier oü il n'y a jamais de segments.

    - La fonction passLongSides guide le robot pour passer les côtés longs de la zone:
        - Elle peut détecter un segment de ligne, utilise la fonction mapLine pour cartographier le segment, 
        puis enregistre la longueur et la position du segment.

    - La fonction endMapping vérifie si à la fois le capteur du milieu et celui du côté long gauche détectent la ligne, ensuite le robot tourne le coin 
    dans la direction vers laquelle il doit se diriger après la fin de la cartographie.

    - La fonction main est le point d'entrée du programme:
        - Elle exécute une séquence initiale pour démarrer le système.
        - Elle guide le robot à travers les côtés courts et longs de la zone, cartographiant les lignes détectées.
        - Elle termine la cartographie de la zone et active les interruptions externes.
        - Dans une boucle infinie, elle attend et gère les états du système, transférant les données cartographiques via 
        la transmission infrarouge ou UART selon l'état actuel.
        - Le main est divisé en états qui représentent les différentes tâches du robot 2.


Concernant la sous-section lib:
- Elle est presque entierement formee de class pour gérer la prise de mesure, la LED, le moteur, la gestion de la minuterie d'interruption...
-  Toute la librairie est composé de headers dans lequel une description de chaque class et les ports utilisée.
