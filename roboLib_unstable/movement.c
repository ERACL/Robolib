#pragma config(Sensor, S2,     sensorFront,    sensorNone)
#pragma config(Motor,  motorA,          motorLeft,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          motorRight,    tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#ifndef MOVEMENT
#define MOVEMENT

#include "config.c"
#include "position.c"

void moveTo(float x, float y); //x et y en mm, positions absolues (cible)

void moveTo_forward(float x, float y); //Force le robot a aller vers l'avant dans le mouvement

void moveTo_backward(float x, float y); //Force le robot a aller vers l'arriere dans le mouvement

void rotateTo(float orientation); //orientation en degres

void pauseMovement();

void resumeMovement();

void abortMovement();

enum movementState {NOMVT, ONGOING, PAUSED, ABORTED};
enum movementState getMovementState();

enum movementType {MOVETO, MOVETO_FORWARD, MOVETO_BACKWARD, ROTATETO};
enum movementType getMovementType();

//-- DEBUT DES DEFINITIONS --


enum movementState __mvtState = NOMVT;

enum movementState getMovementState() { return __mvtState; }

enum movementType __mvtType = MOVETO;

enum movementType getMovementType() { return __mvtType; }

void pauseMovement() { __mvtState = PAUSED; }

void resumeMovement() { if (__mvtState == PAUSED) { __mvtState = ONGOING; } }

void abortMovement() { __mvtState = ABORTED; }

float limit(float number, float limit) {
	if (number > limit)
		return limit;
	if (number < -limit)
		return -limit;
	return number;
}

float limit(float number, float lowerLimit, float upperLimit) {
	if (number > upperLimit)
		return upperLimit;
	if (number < lowerLimit)
		return lowerLimit;
	return number;
}

float mod(float number, float mod) {
	while (number > mod / 2)
		number -= mod;
	while (number < -mod / 2)
		number += mod;
	return number;
}

float ipow(float number, int exponent) {
	if (number == 0)
		return 0;
	float result = 1;
	for (int i = 0; i < (exponent < 0 ? -exponent : exponent); i++)
		result *= number;
	if (exponent < 0)
		result = 1 / result;
	return result;
}

float __targetX = 0; //mm
float __targetY = 0; //mm
float __targetOrientation = 0; //rad

task controlMovement() {
	struct Config const* c = NULL;
	getConfig(&c);
	struct PosData pos;
	getRawPosition(&pos);
	pos.x *= c->mmPerEncode;
	pos.y *= c->mmPerEncode;

	//Initialisation des donnees utiles a l'asservissement
	//i_donnee : integrale de la donnee
	//old_donnee : donnee du cycle d'asservissement precedent (utile pour les derivees)

	//Distance a la cible (mm)
	float dist = sqrt(ipow(pos.x - __targetX, 2) + ipow(pos.y - __targetY, 2));

	//Distance a l'orientation cible (mm)
	float angleDist = mod(__targetOrientation - pos.orientation, 2 * PI) * c->betweenWheels / 2;
	float i_angleDist = 0;

	//Angle a parcourir pour s'aligner avec la cible (rad)
	float oriDiff = PI - pos.orientation;
	if (__targetX - pos.x + dist != 0)
		oriDiff = mod(2 * atan((__targetY - pos.y) / (__targetX - pos.x + dist)) - pos.orientation, 2 * PI);

	//Distance a la cible ponderee par l'orientation (mm)
	float dist_adapted = dist * ipow(cos(oriDiff), c->anglePriorityFactor*2+1);
	float i_dist_adapted = 0;

	//Position des moteurs (permet d'obtenir les vitesses) (mm)
	int posRight = nMotorEncoder[motorRight] * (c->rightMotorReversed ? -1 : 1) * c->mmPerEncode;
	int posLeft = nMotorEncoder[motorLeft] * (c->leftMotorReversed ? -1 : 1) * c->mmPerEncode;
	int old_posRight = posRight;
	int old_posLeft = posLeft;

	//Vitesse de consigne obtenue par asservissement en position (mm / ms)
	float v_str = 0;
	float v_rot = 0;

	//Difference entre vitesse de consigne et vitesse r�elle (mm / ms)
	float vDiff_str = v_str - (float)(posRight + posLeft - old_posRight - old_posLeft) / 2 / c->controlPeriod;
	float vDiff_rot = v_rot - (float)((posRight - old_posRight) - (posLeft - old_posLeft)) / 2 / c->controlPeriod;
	float i_vDiff_str = 0;
	float i_vDiff_rot = 0;

	//Puissance envoyee aux moteurs obtenue par asservissement en vitesse (pow)
	float p_str = 0;
	float p_rot = 0;

	//Compteur de cycles passes (evite d'utiliser un timer)
	unsigned int counter = 0;

	do {
		wait1Msec(c->controlPeriod);

		//Mise a jour de la position
		getRawPosition(&pos);
		pos.x *= c->mmPerEncode;
		pos.y *= c->mmPerEncode;

		//Mise a jour des variables integrees (partie 1)
		i_vDiff_str += vDiff_str * c->controlPeriod / 2;
		i_vDiff_rot += vDiff_rot * c->controlPeriod / 2;
		i_angleDist = fabs(angleDist) < c->integDist ? i_angleDist + angleDist * c->controlPeriod / 2 : 0;
		i_dist_adapted = fabs(dist_adapted) < c->integDist ? i_dist_adapted + dist_adapted * c->controlPeriod / 2 : 0;

		//Mise a jour des variables instantanees
		dist = sqrt(ipow(pos.x - __targetX, 2) + ipow(pos.y - __targetY, 2));
		angleDist = mod(__targetOrientation - pos.orientation, 2 * PI) * c->betweenWheels / 2;
		oriDiff = PI - pos.orientation;
		if (__targetX - pos.x + dist != 0)
			oriDiff = mod(2 * atan((__targetY - pos.y) / (__targetX - pos.x + dist)) - pos.orientation, 2 * PI);
		dist_adapted = dist * ipow(cos(oriDiff), c->anglePriorityFactor*2+1);
		posRight = nMotorEncoder[motorRight] * (c->rightMotorReversed ? -1 : 1) * c->mmPerEncode;
		posLeft = nMotorEncoder[motorLeft] * (c->leftMotorReversed ? -1 : 1) * c->mmPerEncode;
		vDiff_str = v_str - (float)(posRight + posLeft - old_posRight - old_posLeft) / 2 / c->controlPeriod;
		vDiff_rot = v_rot - (float)((posRight - old_posRight) - (posLeft - old_posLeft)) / 2 / c->controlPeriod;

		//Mise a jour des variables integrees (partie 2)
		i_vDiff_str += vDiff_str * c->controlPeriod / 2;
		i_vDiff_rot += vDiff_rot * c->controlPeriod / 2;
		i_angleDist = fabs(angleDist) < c->integDist ? i_angleDist + angleDist * c->controlPeriod / 2 : 0;
		i_dist_adapted = fabs(dist_adapted) < c->integDist ? i_dist_adapted + dist_adapted * c->controlPeriod / 2 : 0;

		//Mise a jour du compteur (hors pause)
		if (__mvtState != PAUSED)
			counter += 1;

		//On arrete le robot en cas de cible atteinte, de pause, d'arret d'urgence ou de timeout
		bool shouldStop = __mvtType != ROTATETO && dist < c->dist_closeEnough;
		shouldStop |= __mvtType == ROTATETO && fabs(angleDist) < c->dist_closeEnough;
		shouldStop |= __mvtState == PAUSED;
		shouldStop |= __mvtState == ABORTED;
		shouldStop |= (unsigned int) (counter * c->controlPeriod) > c->timeout;

		if (shouldStop) {
			v_rot = 0;
			v_str = 0;
			p_str = (int)limit(c->KPVit * vDiff_str + c->KIVit * i_vDiff_str, c->maxPower);
			p_rot = (int)limit(c->KPVit * vDiff_rot + c->KIVit * i_vDiff_rot, c->maxPower);

			if (posRight == old_posRight && posLeft == old_posLeft && __mvtState != PAUSED) {
				p_str = 0;
				p_rot = 0;
				__mvtState = NOMVT;
				//Attention : il faut attendre un petit temps avant de lancer un autre mouvement ;
				//sinon, __mvtState peut etre remodifie avant la fin de la boucle while.
				//S'ensuivent deux asservissements simultanes et donc un gros bazar.
			}
		}
		else {

			//Asservissement en position : obtention de la vitesse de consigne voulue
			if (__mvtType == ROTATETO) {
				v_str = limit(v_str + limit(c->KPPos * dist_adapted - v_str, c->maxAccel * c->controlPeriod), c->maxSpeed);
				v_rot = limit(v_rot + limit(c->KPPos * angleDist + c->KIPos * i_angleDist - v_rot, c->maxAccel * c->controlPeriod), c->maxSpeed);
			}
			else {
				//Cas ou le robot choisit si il avance ou recule
				if (__mvtType == MOVETO || dist < c->integDist) {
					v_str = limit(v_str + limit(c->KPPos * dist_adapted + c->KIPos * i_dist_adapted - v_str, c->maxAccel * c->controlPeriod), c->maxSpeed);
					v_rot = limit(v_rot + limit(1 / (1 + 2 * c->dist_closeEnough / (dist < 0.001 ? 0.001 : dist)) * (c->KPPos * mod(oriDiff, PI) * c->betweenWheels / 2 - v_rot), c->maxAccel * c->controlPeriod), c->maxSpeed);
				}
				else if (__mvtType == MOVETO_FORWARD) {

				}
				else if (__mvtType == MOVETO_BACKWARD) {

				}
			}

			//Asservissement en vitesse : obtention de la vitesse de consigne reelle
			p_str = (int)limit(p_str + limit(c->KPVit * vDiff_str + c->KIVit * i_vDiff_str - p_str, c->maxPowerDerivative * c->controlPeriod), c->maxPower);
			p_rot = (int)limit(p_rot + limit(c->KPVit * vDiff_rot + c->KIVit * i_vDiff_rot - p_rot, c->maxPowerDerivative * c->controlPeriod), c->maxPower);

		}

		//Envoi de la commande aux moteurs
		motor[motorRight] = (c->rightMotorReversed ? -1 : 1) * (p_str + p_rot);
		motor[motorLeft] = (c->leftMotorReversed ? -1 : 1) * (p_str - p_rot);

		//Mise a jour des anciennes variables
		old_posRight = posRight;
		old_posLeft = posLeft;

	} while (__mvtState != NOMVT);

	motor[motorLeft] = 0;
	motor[motorRight] = 0;
}

void moveTo(float x, float y){
	while (__mvtState != NOMVT) { wait1Msec(10); }
	wait1Msec(10);

	__mvtType = MOVETO;
	__mvtState = ONGOING;
	__targetX = x;
	__targetY = y;
	startTask(controlMovement);
}

void moveTo_forward(float x, float y){
	while (__mvtState != NOMVT) { wait1Msec(10); }
	wait1Msec(10);

	__mvtType = MOVETO_FORWARD;
	__mvtState = ONGOING;
	__targetX = x;
	__targetY = y;
	startTask(controlMovement);
}

void moveTo_backward(float x, float y){
	while (__mvtState != NOMVT) { wait1Msec(10); }
	wait1Msec(10);

	__mvtType = MOVETO_BACKWARD;
	__mvtState = ONGOING;
	__targetX = x;
	__targetY = y;
	startTask(controlMovement);
}

void rotateTo(float orientation){
	while (__mvtState != NOMVT) { wait1Msec(10); }
	wait1Msec(10);

	struct PosData pos;
	getPosition(&pos);

	__mvtType = ROTATETO;
	__mvtState = ONGOING;
	__targetX = pos.x;
	__targetY = pos.y;
	__targetOrientation = orientation * PI / 180;
	startTask(controlMovement);
} //orientation en degres



#endif