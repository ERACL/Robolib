#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.h"
#include "position.h"

void initMovement();

/* MovementStates recense les diff�rents �tats possibles d'un mouvement :
 * TBD : mouvement � faire, non commenc� ;
 * ONGOING : mouvement en cours d'�xecution ;
 * DONE : mouvement correctement effectu� ;
 * FAILED_OBS : mouvement �chou� du fait d'un obstacle rencontr� ;
 * FAILED : mouvement �chou� pour une autre raison (batterie faible, etc.).
 * Les �tats TBD et ONGOING ne devraient normalement pas �tre observ�s, mais peuvent �tre utiles pour du d�buggage.
 */
enum MovementState {
	TBD, ONGOING, DONE, FAILED_OBS, FAILED
};

struct MovementResult { //Mettre dans cette structure toutes les donn�es qu'un mouvement doit renvoyer (pour l'instant, juste state)
	enum MovementState state;
};

/* Trois fonctions de mouvement principales : straight, arc et rotate.
 * Elles modifient MovementResult qui contient les donn�es du d�placement.
 * Il faudra �ventuellement relancer un mouvement � la suite de l'ex�cution d'une fonction de mouvement (si il ne s'est pas effectu� correctement).
 * On s'appuiera alors sur getPosition et sur des donn�es externes d'emplacement de la cible.
 */

void straight(float distance);

void arc(float distance, float angle);

void rotate(float angle);

void abort();

void getMovementResult(struct MovementResult const** r);


//-- DEBUT DES DEFINITIONS --


// ---------- CONSTANTES A MODIFIER ----------

/* minSpeed : vitesse minimale pour laquelle laquelle les moteurs bougent.
 * maxAcceleration : acc�l�ration maximale permettant d'�viter les glissements.
 * maxSpeed : vitesse plafond du robot.
 * securityDistance : distance � un obstacle que le robot essaiera de respecter.
 * Une distance de s�curit� trop faible fera �ventuellement d�passer maxAcceleration.
 * errorMarginDistance : marge d'erreur acceptable en distance � une cible � la fin du mouvement.
 * errorMarginAngle : marge d'erreur acceptable en angle � une cible � la fin du mouvement.
 *
 * Distances en mm, temps en s, angles en degr�s.
 */

const float minSpeed = 50;
const float maxAcceleration = 700;
const float maxSpeed = 1000;
const float securityDistance = 50;
const float errorMarginDistance = 10;
const float errorMarginAngle = 2;

// ---------- FIN DES CONSTANTES A MODIFIER ----------

struct MovementResult __result;

void initMovement() { //Cr�e une struct par d�faut pour MovementResult
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
