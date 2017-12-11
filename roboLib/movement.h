#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.h"
#include "position.h"

/* Bibliothèque de mouvement haut-niveau et bas niveau.
 * La partie bas niveau effectue des déplacements en s'arrêtant en cas d'obstacles, et gère l'asservissement des moteurs.
 * La partie haut niveau permet au robot d'aller d'un point A à un point B en gérant les obstacles de façon optimale.
 */

//Partie haut niveau

const float timeout = 10000;

/* moveTo() trouve les mouvements individuels optimaux pour aller jusqu'à la destination.
 * les obstacles sont gérés (et le mouvement retenté) tant que le mouvement n'a pas excédé la valeur de timeout, auquel cas false sera renvoyé.
 * si ignoreOrientation est vrai, le robot ne cherchera pas à se mettre dans l'axe spécifié dans target.
 */
bool moveTo(struct PosData const* target, bool ignoreOrientation = false);

//moveTo préfère utiliser arc() pour économiser des mouvements ; ici, des straight() seront utilisés.
bool moveTo_forceStraight(struct PosData const* target, bool ignoreOrientation = false);

//Partie bas-niveau

/* MovementStates recense les différents états possibles d'un mouvement :
 * TBD : mouvement à faire, non commencé ;
 * ONGOING : mouvement en cours d'exécution ;
 * DONE : mouvement correctement effectué ;
 * FAILED_OBS : mouvement échoué du fait d'un obstacle rencontré ;
 * FAILED_OTHER : mouvement échoué pour une autre raison (batterie faible, etc.).
 * Les états TBD et ONGOING ne devraient normalement pas être observés, mais peuvent être utiles pour du débuggage.
 */
enum MovementState {
	TBD, ONGOING, DONE, FAILED_OBS, FAILED_OTHER
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

void arc(float distance, float angle); //La distance est la distance entre le point de départ et le point d'arrivée (pas la distance parcourure).

void rotate(float angle);

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

//Angle maximal pour lequel le robot choisit d'effectuer un arc de cercle plutôt que de tourner sur soi-même avant, en degrés, durant l'exécution de moveTo().
const float maxAngleForArc = 35;

// ---------- FIN DES CONSTANTES A MODIFIER ----------

struct MovementResult __result;

//TODO : gérer les échecs et le timeout.
bool moveTo(struct PosData const* target, bool ignoreOrientation) {
	struct PosData const* pos = NULL;
	getPosition(&pos);

	float targetDirection = atan((target->y - pos->y) / (target->x - pos->x)) * 180 / PI;
	float deltaOToFace = targetDirection - pos->orientation; //Différence entre l'orientation que le robot aurait si dirigé vers la cible et son orientation actuelle.
	float deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));

	if (deltaOToFace < maxAngleForArc) { //On économise une rotation avant le mouvement vers la cible.
		arc(deltaD, deltaOToFace);
		getPosition(&pos);
		if (!ignoreOrientation)
			rotate(target->orientation - pos->orientation);
	}
	else if (!ignoreOrientation && target->orientation - targetDirection < maxAngleForArc) { //On économise une rotation après le mouvement vers la cible.
		rotate(target->orientation + 2 * (targetDirection - target->orientation));
		getPosition(&pos); //On actualise la position et les deltas, même si deltaD ne devrait pas avoir changé, au cas où.
		deltaOToFace = targetDirection - pos->orientation;
		deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
		arc(deltaD, deltaOToFace);
	}
	else { //On effectue les trois mouvements sans conditions exceptionnelles (deux si ignoreOrientation est vrai).
		rotate(deltaOToFace);
		getPosition(&pos);
		deltaOToFace = targetDirection - pos->orientation;
		deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
		arc(deltaD, deltaOToFace); //On effectue tout de même un arc, au cas où.
		if (!ignoreOrientation) {
			getPosition(&pos);
			rotate(target->orientation - pos->orientation);
		}
	}

	return true;
}

bool moveTo_forceStraight(struct PosData const* target, bool ignoreOrientation) {
	struct PosData const* pos = NULL;
	getPosition(&pos);
	float deltaOToFace = atan((target->y - pos->y) / (target->x - pos->x)) * 180 / PI - pos->orientation;
	float deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
	rotate(deltaOToFace);
	straight(deltaD);
	if (!ignoreOrientation) {
		getPosition(&pos);
		rotate(target->orientation - pos->orientation);
	}

	return true;
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

void getMovementResult(struct MovementResult const** r) {
	*r = &__result;
};

#endif
