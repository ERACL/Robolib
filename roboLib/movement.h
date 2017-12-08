#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.h"
#include "position.h"

void initMovement();

/* MovementStates recense les différents états possibles d'un mouvement :
 * TBD : mouvement à faire, non commencé ;
 * ONGOING : mouvement en cours d'éxecution ;
 * DONE : mouvement correctement effectué ;
 * FAILED_OBS : mouvement échoué du fait d'un obstacle rencontré ;
 * FAILED : mouvement échoué pour une autre raison (batterie faible, etc.).
 * Les états TBD et ONGOING ne devraient normalement pas être observés, mais peuvent être utiles pour du débuggage.
 */
enum MovementState {
	TBD, ONGOING, DONE, FAILED_OBS, FAILED
};

struct MovementResult { //Mettre dans cette structure toutes les données qu'un mouvement doit renvoyer (pour l'instant, juste state)
	enum MovementState state;
};

/* Trois fonctions de mouvement principales : straight, arc et rotate.
 * Elles modifient MovementResult qui contient les données du déplacement.
 * Il faudra éventuellement relancer un mouvement à la suite de l'exécution d'une fonction de mouvement (si il ne s'est pas effectué correctement).
 * On s'appuiera alors sur getPosition et sur des données externes d'emplacement de la cible.
 */

void straight(float distance);

void arc(float distance, float angle);

void rotate(float angle);

void abort();

void getMovementResult(struct MovementResult const** r);


//-- DEBUT DES DEFINITIONS --


// ---------- CONSTANTES A MODIFIER ----------

/* minSpeed : vitesse minimale pour laquelle laquelle les moteurs bougent.
 * maxAcceleration : accélération maximale permettant d'éviter les glissements.
 * maxSpeed : vitesse plafond du robot.
 * securityDistance : distance à un obstacle que le robot essaiera de respecter.
 * Une distance de sécurité trop faible fera éventuellement dépasser maxAcceleration.
 * errorMarginDistance : marge d'erreur acceptable en distance à une cible à la fin du mouvement.
 * errorMarginAngle : marge d'erreur acceptable en angle à une cible à la fin du mouvement.
 *
 * Distances en mm, temps en s, angles en degrés.
 */

const float minSpeed = 50;
const float maxAcceleration = 700;
const float maxSpeed = 1000;
const float securityDistance = 50;
const float errorMarginDistance = 10;
const float errorMarginAngle = 2;

// ---------- FIN DES CONSTANTES A MODIFIER ----------

struct MovementResult __result;

void initMovement() { //Crée une struct par défaut pour MovementResult
	__result.state = TBD;
}

void straight(float distance) {
	bool targetReached = false;
	bool shouldAbort = false;
	while (!targetReached && !shouldAbort) {
		motor[motorB]=40;
		motor[motorA]=-30;
		wait1Msec(1500);
		motor[motorB]=0;
		motor[motorA]=0;
		targetReached = true;
	}
};

void arc(float distance, float angle) {
	bool targetReached = false;
	bool shouldAbort = false;
	while (!targetReached && !shouldAbort) {

	}

};

void rotate(float angle) {
	bool targetReached = false;
	bool shouldAbort = false;
	while (!targetReached && !shouldAbort) {

	}

};

void abort() {

};

void getMovementResult(struct MovementResult const** r) {
	*r = &__result;
};

#endif
