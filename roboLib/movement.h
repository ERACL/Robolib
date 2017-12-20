#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.h"
#include "position.h"

/* Bibliotheque de mouvement haut-niveau et bas niveau.
 * La partie bas niveau effectue des deplacements en s'arretant en cas d'obstacles, et gere l'asservissement des moteurs.
 * La partie haut niveau permet au robot d'aller d'un point A a un point B en gerant les obstacles de façon optimale.
 */

//Partie haut niveau

const float timeout = 10000;

/* moveTo() trouve les mouvements individuels optimaux pour aller jusqu'a la destination.
 * les obstacles sont geres (et le mouvement retente) tant que le mouvement n'a pas excede la valeur de timeout, auquel cas false sera renvoye.
 * si ignoreOrientation est vrai, le robot ne cherchera pas a se mettre dans l'axe specifie dans target.
 */
bool moveTo(struct PosData const* target, bool ignoreOrientation = false);

//moveTo préfère utiliser arc() pour economiser des mouvements ; ici, des straight() seront utilises.
bool moveTo_forceStraight(struct PosData const* target, bool ignoreOrientation = false);

//Partie bas-niveau

/* MovementStates recense les differents etats possibles d'un mouvement :
 * TBD : mouvement a faire, non commence ;
 * ONGOING : mouvement en cours d'execution ;
 * DONE : mouvement correctement effectue ;
 * FAILED_OBS : mouvement echoue du fait d'un obstacle rencontre ;
 * FAILED_OTHER : mouvement echoue pour une autre raison (batterie faible, etc.).
 * Les etats TBD et ONGOING ne devraient normalement pas etre observes, mais peuvent etre utiles pour du debuggage.
 */
enum MovementState {
	TBD, ONGOING, DONE, FAILED_OBS, FAILED_OTHER
};

struct MovementResult { //Mettre dans cette structure toutes les donnees qu'un mouvement doit renvoyer (pour l'instant, juste state)
	enum MovementState state;
};

/* Trois fonctions de mouvement principales : straight, arc et rotate.
 * Elles modifient MovementResult qui contient les donnees du déplacement.
 * Il faudra eventuellement relancer un mouvement a la suite de l'execution d'une fonction de mouvement (s'il ne s'est pas effectue correctement).
 * On s'appuiera alors sur getPosition et sur des donnees externes d'emplacement de la cible.
 */

void straight(float distance);

void arc(float distance, float angle); //La distance est la distance entre le point de depart et le point d'arrivee (pas la distance parcourue).

void rotate(float angle);

void getMovementResult(struct MovementResult const** r);


//-- DEBUT DES DEFINITIONS --

// ---------- CONSTANTES A MODIFIER ----------

/* minSpeed : vitesse minimale pour laquelle laquelle les moteurs bougent.
 * maxAcceleration : accelération maximale permettant d'eviter les glissements.
 * maxSpeed : vitesse plafond du robot.
 * securityDistance : distance a un obstacle que le robot essaiera de respecter.
 * Une distance de securite trop faible fera eventuellement depasser maxAcceleration.
 * errorMarginDistance : marge d'erreur acceptable en distance a une cible a la fin du mouvement.
 * errorMarginAngle : marge d'erreur acceptable en angle a une cible a la fin du mouvement.
 *
 * Distances en mm, temps en s, angles en degres.
 */

const float minSpeed = 50;
const float maxAcceleration = 700;
const float maxSpeed = 1000;
const float securityDistance = 50;
const float errorMarginDistance = 10;
const float errorMarginAngle = 2;

//Angle maximal pour lequel le robot choisit d'effectuer un arc de cercle plutot que de tourner sur soi-meme avant, en degres, durant l'execution de moveTo().
const float maxAngleForArc = 35;

// ---------- FIN DES CONSTANTES A MODIFIER ----------

struct MovementResult __result;

//TODO : gerer les echecs et le timeout.
bool moveTo(struct PosData const* target, bool ignoreOrientation) {
	struct PosData const* pos = NULL;
	getPosition(&pos);

	float targetDirection = atan((target->y - pos->y) / (target->x - pos->x)) * 180 / PI;
	float deltaOToFace = targetDirection - pos->orientation; //Difference entre l'orientation que le robot aurait si dirige vers la cible et son orientation actuelle.
	float deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));

	if (deltaOToFace < maxAngleForArc) { //On economise une rotation avant le mouvement vers la cible.
		arc(deltaD, deltaOToFace);
		getPosition(&pos);
		if (!ignoreOrientation)
			rotate(target->orientation - pos->orientation);
	}
	else if (!ignoreOrientation && target->orientation - targetDirection < maxAngleForArc) { //On economise une rotation apres le mouvement vers la cible.
		rotate(target->orientation + 2 * (targetDirection - target->orientation));
		getPosition(&pos); //On actualise la position et les deltas, meme si deltaD ne devrait pas avoir change, au cas ou.
		deltaOToFace = targetDirection - pos->orientation;
		deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
		arc(deltaD, deltaOToFace);
	}
	else { //On effectue les trois mouvements sans conditions exceptionnelles (deux si ignoreOrientation est vrai).
		rotate(deltaOToFace);
		getPosition(&pos);
		deltaOToFace = targetDirection - pos->orientation;
		deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
		arc(deltaD, deltaOToFace); //On effectue tout de meme un arc, au cas ou.
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
