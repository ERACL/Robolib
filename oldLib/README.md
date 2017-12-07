Librairie de base LEGO NXT/EV3
==============================


Prérequis
---------
Avoir lu le tutoriel RobotC sur l'ERACL Box pour être un minimum familié avec les syntaxes et les tasks (très importantes).
Etre familier avec la notion d'odométrie.

Introduction
------------
Cette librairie, compatible avec les briques NXT, EV3, Matrix et Tetrix permet de faire des mouvements simples pour le robot : avancer tout droit d'une distance donnée et tourner sur place d'un angle donné.

Conventions
-----------
*Unitées :*
Tous les temps sont exprimes en ms.
Toutes les distances sont exprimees en mm.

*Convention moteurs :*
 - motorA = moteur droit
 - motorB = moteur gauche
Le nom des moteurs peut être modifié dans le logiciel RobotC si les branchements ne sont pas ceux voulus.

*Convention capteurs :*
 - SonarAvant = capteur ultrason avant
 - SonarArriere = capteur ultrason arrière

Par convention, les noms des tasks finissent par T. (ex : task blablaT, void blabla)

Important
---------
Ne jamais modifier les valeurs de nMotorEncoder[], sauf a l'initialisation pour le mettre a 0 dans la task main.

Idée de la librairie
--------------------
Les fonctions de bas niveau viennent en premier dans le code, ensuite viennent les fonctions de plus haut niveau. Le code est bati par "couches" et respecte les idées de l'encapsulation (comme dans les couches réseau du modèle OSI). Ceci est un choix et il peut être discuté et modifié. Ainsi, on aura, par appel de fonctions : 
task main -> avance_nulle_anyway -> avance_nulle -> avance_nulle_private (-> avance_nulle_obstacle si un obstacle est rencontré)

Cette stratégie a les avantages de bien diviser et organiser le code, d'aider aux diagnostics et de concevoir son code en pyramide. Les parties haut niveau gèreront le mouvement macroscopiquement parlant et les parties bas niveau le gèreront microscopiquement parlant.

Plan de la librairie
--------------------

I/ Fonctions d'avancement
1. avance_nulle_obstacle, recule_nulle_obstacle (couche 1)
2. avance_nulle_private, recule_nulle_private (couche 1)
3. avance_nulle, recule_nulle (fonctions et tasks) (couche 2)
4. avance_nulle_anyway, recule_nulle_anyway (couche 2)
5. avance_nulle_fairplay, recule_nulle_fairplay (couche 3)

II/ Fonctions de rotation
1. turnDeg_private (couche 1) (**A refaire !!!! URGEMMENT !**)
2. turnRightDeg, turnLeftDeg (couche 2)

**Pour un detail des fonctions et des variables de la librairie, voir la fiche technique associée sur le wiki et le code de la librairie.**

Pistes d'amélioration
---------------------
Faire une tâche qui aura pour rôle de mesurer le mouvement des roues et d'en déduire la position exacte du robot sur le terrain. L'idée est de pouvoir paramétrer le mouvement du robot par rapport à sa position réelle et non pas par rapport à ce que l'on pense avoir fait (chaque mouvement amenant une imprécision).

Contributeurs majeurs
---------------------
 - Alexandre MARSONE

