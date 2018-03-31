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

enum movementType {MOVETO, MOVETO_BACKWARD, ROTATETO};
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

float mod2Pi(float number) {
	while (number > PI)
		number -= 2 * PI;
	while (number < -PI)
		number += 2 * PI;
	return number;
}

float modPi(float number) {
	while (number > PI / 2)
		number -= PI;
	while (number < -PI / 2)
		number += PI;
	return number;
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
	float dist = sqrt(pow(__targetX - pos.x, 2) + pow(__targetY - pos.y, 2));
	float old_dist = dist;

	//Angle a parcourir pour atteindre l'angle cible (rad) ; nul si le mouvement ne demande pas de rotation
	float angle = (__mvtType == ROTATETO) ? mod2Pi(__targetOrientation - pos.orientation) : 0;
	float old_angle = angle;

	//Angle a parcourir pour s'aligner avec la cible (rad)
	float angleTarget = PI - pos.orientation;
	if (__targetX - pos.x + dist != 0)
		angleTarget = mod2Pi(2 * atan((__targetY - pos.y) / (__targetX - pos.x + dist)) - pos.orientation);
	float i_angleTarget = 0;
	float old_angleTarget = angleTarget;

	//Distance a la cible ponderee par l'angle (negative si vers l'arriere, nulle si perpendiculaire) (mm)
	float dist_weighted = cos(angleTarget) * dist;
	float i_dist_weighted = 0;
	float old_dist_weighted = dist_weighted;

	//Angle a la cible qui ignore la direction (avant / arriere) (rad)
	float angleTarget_modPi = modPi(angleTarget);
	float i_angleTarget_modPi = 0;
	float old_angleTarget_modPi = angleTarget_modPi;


	//Vitesse de consigne, decomposee en vitesse de rotation et d'avance droite (mm / ms)
	float vc_rot = 0;
	float vc_str = 0;

	//Puissance envoy�e aux moteurs (pow)
	float p_rot = 0;
	float p_str = 0;

	//Compteur de cycles passes (evite d'utiliser un timer)
	unsigned int counter = 0;

	do {
		wait1Msec(c->controlPeriod);

		//Mise a jour de la position
		getRawPosition(&pos);
		pos.x *= c->mmPerEncode;
		pos.y *= c->mmPerEncode;

		//Mise a jour des variables instantanees
		dist = sqrt(pow(__targetX - pos.x, 2) + pow(__targetY - pos.y, 2));
		angle = (__mvtType == ROTATETO) ? mod2Pi(__targetOrientation - pos.orientation) : 0;
		angleTarget = PI - pos.orientation;
		if (__targetX - pos.x + dist != 0)
			angleTarget = mod2Pi(2 * atan((__targetY - pos.y) / (__targetX - pos.x + dist)) - pos.orientation);
		dist_weighted = cos(angleTarget) * dist;
		angleTarget_modPi = modPi(angleTarget);

		//Mise a jour des variables integrees
		i_dist_weighted = limit(i_dist_weighted + dist_weighted * c->controlPeriod, c->integLimit / c->KIPos);
		i_angleTarget = limit(i_angleTarget + angleTarget * c->controlPeriod, c->integLimit / (c->KIPos * c->betweenWheels / 2));
		i_angleTarget_modPi = limit(i_angleTarget_modPi + angleTarget_modPi * c->controlPeriod, c->integLimit / (c->KIPos * c->betweenWheels / 2));

		//Mise a jour du compteur (hors pause)
		if (__mvtState != PAUSED)
			counter += 1;

		//On arrete le robot en cas de cible atteinte, de pause, d'arret d'urgence ou de timeout
		bool shouldStop = (dist < c->dist_closeEnough) && (fabs(angle) < c->angle_closeEnough);
		shouldStop |= __mvtState == PAUSED;
		shouldStop |= __mvtState == ABORTED;
		shouldStop |= counter > (unsigned int)(10000 / c->controlPeriod);

		if (shouldStop) {
			p_str -= limit(p_str, c->maxAccel * c->controlPeriod);
			p_rot -= limit(p_rot, c->maxAccel * c->controlPeriod);

			if (dist == old_dist && angle == old_angle && __mvtState != PAUSED) {
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

			//Differentielles de vitesse (facilitent la limite d'acceleration)
			//Les limitations en acceleration et en vitesse s'effectuent separement sur les vitesses d'avance droite et de rotation ;
			//cela permet d'eviter de perdre de l'information sur le mouvement, mais fait que la limite est parfois plus basse que demandee.
			float d_vc_str = 0, d_vc_rot = 0;

			if (__mvtType == ROTATETO) {

			}
			else {
				d_vc_str = limit(c->KPPos * dist_weighted
											 + c->KIPos * i_dist_weighted
											 + limit(c->KDPos * (dist_weighted - old_dist_weighted) / c->controlPeriod, c->derivLimit)
											 - vc_str, c->maxAccel * c->controlPeriod * 0.6);

				//Cas ou le robot a le choix d'aller en avant ou en arriere
				if (__mvtType == MOVETO || c->dist_allowBackward < 0 || dist < c->dist_allowBackward) {
					d_vc_rot = limit(c->KPPos * c->betweenWheels / 2 * angleTarget_modPi
												 + c->KIPos * c->betweenWheels / 2 * i_angleTarget_modPi
												 + limit(c->KDPos * c->betweenWheels / 2 * (angleTarget_modPi - old_angleTarget_modPi) / c->controlPeriod, c->derivLimit)
												 - vc_rot, c->maxAccel * c->controlPeriod - fabs(d_vc_str)); //limite au pire a 0.4*maxAccel

					vc_str = limit(vc_str + d_vc_str, c->maxSpeed * 0.6);
					vc_rot = limit(vc_rot + d_vc_rot, c->maxSpeed - fabs(vc_str));
				}
				//Cas ou le robot doit aller dans un sens specifique
				else {
					if (__mvtType == MOVETO_FORWARD) {
						d_vc_rot = limit(c->KPPos * c->betweenWheels / 2 * angleTarget
													 + c->KIPos * c->betweenWheels / 2 * i_angleTarget
													 + limit(c->KDPos * c->betweenWheels / 2 * (angleTarget - old_angleTarget) / c->controlPeriod, c->derivLimit)
													 - vc_rot, c->maxAccel * c->controlPeriod - fabs(d_vc_str));

						vc_str = limit(vc_str + d_vc_str, 0, c->maxSpeed * 0.6);
						vc_rot = limit(vc_rot + d_vc_rot, c->maxSpeed - fabs(vc_str));
					}
					else if (__mvtType == MOVETO_BACKWARD) {
						d_vc_rot = limit(c->KPPos * c->betweenWheels / 2 * angleTarget_backward
													 + c->KIPos * c->betweenWheels / 2 * i_angleTarget_backward
													 + limit(c->KDPos * c->betweenWheels / 2 * (angleTarget - old_angleTarget) / c->controlPeriod, c->derivLimit)
													 - vc_rot, c->maxAccel * c->controlPeriod - fabs(d_vc_str));

						vc_str = limit(vc_str + d_vc_str, 0, c->maxSpeed * 0.6);
						vc_rot = limit(vc_rot + d_vc_rot, c->maxSpeed - fabs(vc_str));

					}
				}
			}

			//Asservissement en vitesse : obtention de la consigne reelle

		}

		//Envoi de la commande aux moteurs
		motor[motorRight] = (c->rightMotorReversed ? -1 : 1) * (p_str + p_rot);
		motor[motorLeft] = (c->leftMotorReversed ? -1 : 1) * (p_str - p_rot);

		//Mise a jour des anciennes variables
		old_dist = dist;
		old_angle = angle;
		old_dist_weighted = dist_weighted;
		old_angleTarget = angleTarget;
		old_angleTarget_modPi = angleTarget_modPi;

	} while (__mvtState != NOMVT);

	motor[motorLeft] = 0;
	motor[motorRight] = 0;
}

void moveTo(float x, float y){} //x et y en mm, positions absolues (cible)

void moveTo_forward(float x, float y){}

void moveTo_backward(float x, float y){}

void rotateTo(float orientation){} //orientation en degres



#endif
