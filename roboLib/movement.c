#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.c"
#include "position.c"

// ---------- CONSTANTES A MODIFIER ----------

//Angle maximal pour lequel le robot choisit d'effectuer un arc de cercle plutot que de tourner sur soi-meme avant, en degres, durant l'execution de moveTo().
const float maxAngleForArc = 35;

const unsigned int controlPeriod = 20; //Temps entre deux asservissements : ms

const float timeOut = 6000; //Temps d'immobilite apres lequel un mouvement est considere echoue : ms
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
bool moveTo(float x, float y);
bool moveTo(float x, float y, float orientation);
bool moveTo(struct PosData const* target, bool ignoreOrientation = false);

//moveTo prefere utiliser arc() pour economiser des mouvements ; ici, des straight() seront utilises.
bool moveTo_forceStraight(float x, float y);
bool moveTo_forceStraight(float x, float y, float orientation);
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

struct MovementResult { //Mettre dans cette structure toutes les donnees qu'un mouvement doit renvoyer.
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
	struct MovementResult const* res = NULL;
	float deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
	float deltaOToFace;
	if (target->y == pos->y && target->y > pos->x)
		deltaOToFace = 180 - pos->orientation;
	else
		deltaOToFace = 2 * atan((target->y - pos->y) / (target->x - pos->x + deltaD)) * 360 / 2 / PI - pos->orientation;

	while (deltaOToFace > 180)
		deltaOToFace -= 360;
	while (deltaOToFace < -180)
		deltaOToFace += 360;

	rotate(deltaOToFace);
	straight(deltaD);
	if (!ignoreOrientation) {
		getPosition(&pos);
		float deltaO = target->orientation - pos->orientation;
		while (deltaO > 180)
			deltaO -= 360;
		while (deltaO < -180)
			deltaO += 360;
		rotate(deltaO);
	}

	do {
		wait1Msec(10);
		getMovementResult(&res);
	}	while(res->state == ONGOING);

	return true;
}

bool moveTo_forceStraight(float x, float y) {
	PosData target;
	target.x = x;
	target.y = y;
	return moveTo_forceStraight(&target, true);
}

bool moveTo_forceStraight(float x, float y, float orientation) {
	PosData target;
	target.x = x;
	target.y = y;
	target.orientation = orientation;
	return moveTo_forceStraight(&target);
}

float limit(float number, float limit) {
	if (number > limit)
		return limit;
	if (number < -limit)
		return -limit;
	return number;
}

float __straight_targetDist; //Distance a atteindre dans la tache straight_control(), en encode (attention, donne en mm dans straight())

task straight_control() {
	struct PosData const* pos = NULL;
	struct Config const* c = NULL;
	getConfig(&c);

	bool integralControl = false;
	float DDist = 0;
	float DDistIntegral = 0;
	float vc = 0;
	float oldDist = 0;
	int ciclesIdle = 0;
	while (__result.state == ONGOING) {
		wait1Msec(controlPeriod);
		getRawPosition(&pos);
		DDist = __straight_targetDist - pos->traveledDistance;

		if (fabs(pos->traveledDistance - oldDist) < 0.01)
			ciclesIdle += 1;
		else
			ciclesIdle = 0;

		if (ciclesIdle * controlPeriod > timeIdleWin && fabs(DDist) < c->errorMarginDistance) {
			__result.state = DONE;
		}
		else {
			if (fabs(DDist) < c->integralControlDistance && integralControl == false)
				integralControl = true;

			if (integralControl)
				DDistIntegral += DDist * controlPeriod;

			vc = limit(vc + limit(c->K_p * DDist + c->K_i * DDistIntegral - vc, c->maxAllowedPowerDerivative * controlPeriod), c->maxAllowedPower);

			oldDist = pos->traveledDistance;
			motor[motorA] = (int)vc;
			motor[motorB] = (int)vc;
		}
	}

	motor[motorA] = 0;
	motor[motorB] = 0;
}

void straight(float distance) {
	while (__result.state == ONGOING) { wait1Msec(10); }
	__result.state = ONGOING;
	__result.type = STRAIGHT;
	struct Config const* c = NULL;
	getConfig(&c);
	struct PosData const* pos = NULL;
	getRawPosition(&pos);
	__straight_targetDist = distance / c->mmPerEncode + pos->traveledDistance;
	startTask(straight_control);
}

float __arc_targetDist;
float __arc_ratioRightOnLeft;

task arc_control() {
	struct PosData const* pos = NULL;
	struct Config const* c = NULL;
	getConfig(&c);

	bool integralControl = false;
	float DDist = 0;
	float DDistIntegral = 0;
	float vc = 0;
	float oldDist = 0;
	int ciclesIdle = 0;
	while (__result.state == ONGOING) {
		wait1Msec(controlPeriod);
		getRawPosition(&pos);
		DDist = __arc_targetDist - pos->traveledDistance;

		if (fabs(pos->traveledDistance - oldDist) < 0.01)
			ciclesIdle += 1;
		else
			ciclesIdle = 0;

		if (ciclesIdle * controlPeriod > timeIdleWin && fabs(DDist) < c->errorMarginDistance) {
			__result.state = DONE;
		}
		else {
			if (fabs(DDist) < c->integralControlDistance && integralControl == false)
				integralControl = true;

			if (integralControl)
				DDistIntegral += DDist * controlPeriod;

			vc = limit(vc + limit(c->K_p * DDist + c->K_i * DDistIntegral - vc, c->maxAllowedPowerDerivative * controlPeriod), c->maxAllowedPower);

			oldDist = pos->traveledDistance;
			motor[motorA] = (int)vc;
			motor[motorB] = (int)vc;
		}
	}

	motor[motorA] = 0;
	motor[motorB] = 0;
}

void arc(float distance, float angle) {
	/* Non fonctionnel pour le moment
	while (__result.state == ONGOING) { wait1Msec(10); }
	__result.state = ONGOING;
	__result.type = ARC;
	struct Config const* c = NULL;
	getConfig(&c);
	struct PosData const* pos = NULL;
	getRawPosition(&pos);
	float arcRadius = distance / (c->mmPerEncode * 2 * sin(angle * PI / 180 / 2));
	__arc_targetDist = (arcRadius + c->betweenWheels / c->mmPerEncode / 2) * angle * PI / 180;
	__arc_ratioRightOnLeft = (arcRadius + c->betweenWheels / c->mmPerEncode / 2) / (arcRadius - c->betweenWheels / c->mmPerEncode / 2);

	startTask(arc_control);
	*/
};

float __rotate_targetOrientation; //Angle a atteindre dans la tache rotate_control(), en radians (attention, donne en degres dans rotate())

task rotate_control() {
	struct PosData const* pos = NULL;
	struct Config const* c = NULL;
	getConfig(&c);

	bool integralControl = false;
	float DDist = 0;
	float DDistIntegral = 0;
	float vc = 0;
	float oldOrientation = 0;
	int ciclesIdle = 0;
	while (__result.state == ONGOING) {
		wait1Msec(controlPeriod);
		getRawPosition(&pos);
		DDist = (__rotate_targetOrientation - pos->orientation) * c->betweenWheels / (2 * c->mmPerEncode);

		if (fabs(pos->orientation - oldOrientation) < 0.0001)
			ciclesIdle += 1;
		else
			ciclesIdle = 0;

		if (ciclesIdle * controlPeriod > timeIdleWin && fabs(DDist / c->betweenWheels * 2 * c->mmPerEncode) < c->errorMarginAngle) {
			__result.state = DONE;
		}
		else {
			if (fabs(DDist) < c->integralControlDistance && integralControl == false)
				integralControl = true;

			if (integralControl)
				DDistIntegral += DDist * controlPeriod;

			vc = limit(vc + limit(c->K_p * DDist + c->K_i * DDistIntegral - vc, c->maxAllowedPowerDerivative * controlPeriod), c->maxAllowedPower);

			oldOrientation = pos->orientation;
			motor[motorA] = -(int)vc;
			motor[motorB] = (int)vc;
		}
	}
	motor[motorA] = 0;
	motor[motorB] = 0;
}

void rotate(float angle) {
while (__result.state == ONGOING) { wait1Msec(10); }
	__result.state = ONGOING;
	__result.type = ROTATE;
	struct Config const* c = NULL;
	getConfig(&c);
	struct PosData const* pos = NULL;
	getRawPosition(&pos);
	__rotate_targetOrientation = pos->orientation + angle * PI / 180;
	startTask(rotate_control);
};

void getMovementResult(struct MovementResult const** r) {
	*r = &__result;
};

#endif
