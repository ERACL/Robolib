#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.h"
#include "position.h"

// ---------- CONSTANTES A MODIFIER ----------

//Angle maximal pour lequel le robot choisit d'effectuer un arc de cercle plutot que de tourner sur soi-meme avant, en degres, durant l'execution de moveTo().
const float maxAngleForArc = 35;

const unsigned int controlPeriod = 20; //Temps entre deux asservissements : ms

const float timeIdleFail = 1000; //Temps d'immobilite apres lequel un mouvement est considere echoue : ms
const float timeIdleWin = 100; //Temps d'immobilite pres de la cible apres lequel un mouvement est considere termine : ms

// ---------- FIN DES CONSTANTES A MODIFIER ----------

/* Bibliotheque de mouvement haut-niveau et bas niveau.
 * La partie bas niveau effectue des deplacements en s'arretant en cas d'obstacles, et gere l'asservissement des moteurs.
 * La partie haut niveau permet au robot d'aller d'un point A a un point B en gerant les obstacles de facon optimale.
 */

//Partie haut niveau

/* moveTo() trouve les mouvements individuels optimaux pour aller jusqu'a la destination.
 * les obstacles sont geres (et le mouvement retente) tant que le mouvement n'a pas excede la valeur de timeout, auquel cas false sera renvoye.
 * si ignoreOrientation est vrai, le robot ne cherchera pas a se mettre dans l'axe specifie dans target.
 */
bool moveTo(struct PosData const* target, bool ignoreOrientation = false);

//moveTo prefere utiliser arc() pour economiser des mouvements ; ici, des straight() seront utilises.
bool moveTo_forceStraight(struct PosData const* target, bool ignoreOrientation = false);

//Partie bas-niveau

/* MovementStates recense les differents etats possibles d'un mouvement :
 * TBD : mouvement a faire, non commence ;
 * ONGOING : mouvement en cours d'execution ;
 * DONE : mouvement correctement effectue ;
 * FAILED_OBS : mouvement echoue du fait d'un obstacle rencontre ;
 * FAILED_TIMEOUT : mouvement echoue du fait d'un temps trop long pour l'action ;
 * FAILED_OTHER : mouvement echoue pour une autre raison ;
 * ABORTED : arrete par un appui sur le bouton rouge.
 */
enum MovementState {
	TBD, ONGOING, DONE, FAILED_OBS, FAILED_TIMEOUT, FAILED_OTHER, ABORTED
};

enum MovementType {
	STRAIGHT, ARC, ROTATE
};

struct MovementResult { //Mettre dans cette structure toutes les donnees qu'un mouvement doit renvoyer (pour l'instant, juste state)
	enum MovementState state;
	enum MovementType type;
};

/* Trois fonctions de mouvement principales : straight, arc et rotate.
 * Elles modifient MovementResult qui contient les donnees du deplacement.
 * Il faudra eventuellement relancer un mouvement a la suite de l'execution d'une fonction de mouvement (s'il ne s'est pas effectue correctement).
 * On s'appuiera alors sur getPosition et sur des donnees externes d'emplacement de la cible.
 */

void straight(float distance);

void arc(float distance, float angle); //La distance est la distance entre le point de depart et le point d'arrivee (pas la distance parcourue).

void rotate(float angle);

void getMovementResult(struct MovementResult const** r);


//-- DEBUT DES DEFINITIONS --

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

/*
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
*/

float limit(float number, float limit) {
	if (number > limit)
		return limit;
	if (number < -limit)
		return -limit;
	return number;
}

float straight_targetDist; //Distance a atteindre dans la tache straight_control(), en encode (attention, donne en mm dans straight())

task straight_control() {
	struct PosData const* pos = NULL;
	struct Config const* c = NULL;
	getConfig(&c);

	bool straight_integralControl = false;
	float DDist = 0;
	float DDistIntegral = 0;
	float vc = 0;
	float oldDist = 0;
	int ciclesIdle = 0;
	while (__result.state == ONGOING) {
		wait1Msec(controlPeriod);
		getRawPosition(&pos);
		DDist = straight_targetDist - pos->traveledDistance;

		if (fabs(pos->traveledDistance - oldDist) < 0.01)
			ciclesIdle += 1;
		else
			ciclesIdle = 0;

		if (ciclesIdle * controlPeriod > timeIdleFail) {
			__result.state = FAILED_TIMEOUT;
			motor[motorA] = 0;
			motor[motorB] = 0;
		}
		if (ciclesIdle * controlPeriod > timeIdleWin && fabs(DDist) < c->errorMarginDistance) {
			__result.state = DONE;
			motor[motorA] = 0;
			motor[motorB] = 0;
		}
		else {
			if (fabs(DDist) < c->integralControlDistance && straight_integralControl == false)
				straight_integralControl = true;

			if (straight_integralControl)
				DDistIntegral += DDist * controlPeriod;

			vc = limit(vc + limit(c->K_p * DDist + c->K_i * DDistIntegral - vc, c->maxAllowedPowerDerivative * controlPeriod), c->maxAllowedPower);

			oldDist = pos->traveledDistance;
			motor[motorA] = (int)vc;
			motor[motorB] = (int)vc;
		}
	}
}

void straight(float distance) {
	while (__result.state == ONGOING) {}
	__result.state = ONGOING;
	__result.type = STRAIGHT;
	struct Config const* c = NULL;
	getConfig(&c);
	struct PosData const* pos = NULL;
	getRawPosition(&pos);
	straight_targetDist = distance / c->mmPerEncode + pos->traveledDistance;
	startTask(straight_control);
}

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
