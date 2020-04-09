
#ifndef MOVEMENT
#define MOVEMENT

#include "config.c"
#include "position.c"

bool didTimeout = false;

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

float min(float n1, float n2) { return n1 < n2 ? n1 : n2; }
float max(float n1, float n2) { return n1 < n2 ? n2 : n1; }

float __targetX = 0; //mm
float __targetY = 0; //mm
float __targetOrientation = 0; //rad

float getDistAdapter(float oriDiff, float criticalOriDiff) {
	if (oriDiff < 0)
		return getDistAdapter(-oriDiff, criticalOriDiff);
	if (oriDiff > PI - criticalOriDiff)
		return -getDistAdapter(PI - oriDiff, criticalOriDiff);
	return max(1 - oriDiff / criticalOriDiff, 0);
}

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
	float dist = sqrt(pow(pos.x - __targetX, 2) + pow(pos.y - __targetY, 2));

	//Distance a l'orientation cible (mm)
	float angleDist = mod(__targetOrientation - pos.orientation, 2 * PI) * c->betweenWheels / 2;
	float i_angleDist = 0;

	//Angle a parcourir pour s'aligner avec la cible (rad)
	float oriDiff = PI - pos.orientation;
	if (__targetX - pos.x + dist != 0)
		oriDiff = mod(2 * atan((__targetY - pos.y) / (__targetX - pos.x + dist)) - pos.orientation, 2 * PI);

	//Distance ponderee par le positionnement du robot et son mouvement (mm)
	float dist_adapted = 0;
	if (fabs(oriDiff) < PI/2) {
		if (__mvtType == MOVETO_BACKWARD && dist > c->dist_allowBackwardTranslation)
			dist_adapted = 0;
		else
			dist_adapted = min(dist, c->maxEffectiveDistTranslation) * getDistAdapter(oriDiff, max(c->dist_closeEnoughTranslation / limit(dist, c->dist_closeEnoughTranslation, c->maxEffectiveDistTranslation), 0.1));
	}
	else {
		if (__mvtType == MOVETO_FORWARD && dist > c->dist_allowBackwardTranslation)
			dist_adapted = 0;
		else
			dist_adapted = min(dist, c->maxEffectiveDistTranslation) * getDistAdapter(oriDiff, max(c->dist_closeEnoughTranslation / limit(dist, c->dist_closeEnoughTranslation, c->maxEffectiveDistTranslation), 0.1));
	}
	float i_dist_adapted = 0;

	//Angle d'alignement adapte pour le mouvement choisi (rad)
	float oriDiff_adapted = 0;
	if (__mvtType == MOVETO_FORWARD && dist > c->dist_allowBackwardRotation)
		oriDiff_adapted = limit(oriDiff, c->maxEffectiveDistRotation / (c->betweenWheels/2));
	else if (__mvtType == MOVETO_BACKWARD && dist > c->dist_allowBackwardRotation)
		oriDiff_adapted = limit(mod(PI + oriDiff, 2*PI), c->maxEffectiveDistRotation / (c->betweenWheels/2));
	else
		oriDiff_adapted = limit(mod(oriDiff, PI), c->maxEffectiveDistRotation / (c->betweenWheels/2));
	float i_oriDiff_adapted = 0;

	//Position des moteurs (permet d'obtenir les vitesses) (mm)
	float posRight = nMotorEncoder[motorRight] * (c->rightMotorReversed ? -1 : 1) * c->mmPerEncode;
	float posLeft = nMotorEncoder[motorLeft] * (c->leftMotorReversed ? -1 : 1) * c->mmPerEncode;
	float old_posRight = posRight;
	float old_posLeft = posLeft;

	//Vitesse de consigne obtenue par asservissement en position (mm / ms)
	float v_str = 0;
	float v_rot = 0;

	//Vitesse de consigne ramen�e aux moteurs (mm / ms)
	float v_right = v_str + v_rot;
	float v_left = v_str - v_rot;

	//Difference entre vitesse de consigne et vitesse r�elle (mm / ms)
	float vDiff_right = v_right - (posRight - old_posRight) / c->controlPeriod;
	float vDiff_left = v_left - (posLeft - old_posLeft) / c->controlPeriod;
	float i_vDiff_right = 0;
	float i_vDiff_left = 0;

	//Puissance envoyee aux moteurs obtenue par asservissement en vitesse (pow)
	float p_right = 0;
	float p_left = 0;

	//Compteur de cycles passes (evite d'utiliser un timer)
	unsigned int counter = 0;

	do {
		wait1Msec(c->controlPeriod);

		//Mise a jour de la position
		getRawPosition(&pos);
		pos.x *= c->mmPerEncode;
		pos.y *= c->mmPerEncode;


		//Mise a jour des variables de position
		//Mise a jour des variables integrees (partie 1)
		i_angleDist = fabs(angleDist) < c->integDistRotation ? i_angleDist + angleDist * c->controlPeriod / 2 : 0;
		i_dist_adapted = fabs(dist_adapted) < c->integDistTranslation ? i_dist_adapted + dist_adapted * c->controlPeriod / 2 : 0;
		i_oriDiff_adapted = fabs(oriDiff_adapted) * c->betweenWheels / 2 < c->integDistRotation ? i_oriDiff_adapted + oriDiff_adapted * c->controlPeriod / 2 : 0;

		//Mise a jour des variables instantanees
		dist = sqrt(pow(pos.x - __targetX, 2) + pow(pos.y - __targetY, 2));
		angleDist = mod(__targetOrientation - pos.orientation, 2 * PI) * c->betweenWheels / 2;
		oriDiff = PI - pos.orientation;
		if (__targetX - pos.x + dist != 0)
			oriDiff = mod(2 * atan((__targetY - pos.y) / (__targetX - pos.x + dist)) - pos.orientation, 2 * PI);

		if (fabs(oriDiff) < PI/2) {
			if (__mvtType == MOVETO_BACKWARD && dist > c->dist_allowBackwardTranslation)
				dist_adapted = 0;
			else
				dist_adapted = min(dist, c->maxEffectiveDistTranslation) * getDistAdapter(oriDiff, max(c->dist_closeEnoughTranslation / limit(dist, c->dist_closeEnoughTranslation, c->maxEffectiveDistTranslation), 0.1));
		}
		else {
			if (__mvtType == MOVETO_FORWARD && dist > c->dist_allowBackwardTranslation)
				dist_adapted = 0;
			else
				dist_adapted = min(dist, c->maxEffectiveDistTranslation) * getDistAdapter(oriDiff, max(c->dist_closeEnoughTranslation / limit(dist, c->dist_closeEnoughTranslation, c->maxEffectiveDistTranslation), 0.1));
		}

		if (__mvtType == MOVETO_FORWARD && dist > c->dist_allowBackwardTranslation)
			oriDiff_adapted = limit(oriDiff, c->maxEffectiveDistRotation / (c->betweenWheels/2));
		else if (__mvtType == MOVETO_BACKWARD && dist > c->dist_allowBackwardTranslation)
			oriDiff_adapted = limit(mod(PI + oriDiff, 2*PI), c->maxEffectiveDistRotation / (c->betweenWheels/2));
		else
			oriDiff_adapted = limit(mod(oriDiff, PI), c->maxEffectiveDistRotation / (c->betweenWheels/2));

		posRight = nMotorEncoder[motorRight] * (c->rightMotorReversed ? -1 : 1) * c->mmPerEncode;
		posLeft = nMotorEncoder[motorLeft] * (c->leftMotorReversed ? -1 : 1) * c->mmPerEncode;

		//Mise a jour des variables integrees (partie 2)
		i_angleDist = fabs(angleDist) < c->integDistRotation ? i_angleDist + angleDist * c->controlPeriod / 2 : 0;
		i_dist_adapted = fabs(dist_adapted) < c->integDistTranslation ? i_dist_adapted + dist_adapted * c->controlPeriod / 2 : 0;
		i_oriDiff_adapted = fabs(oriDiff_adapted) * c->betweenWheels / 2 < c->integDistRotation ? i_oriDiff_adapted + oriDiff_adapted * c->controlPeriod / 2 : 0;

		//Mise a jour du compteur (pour le timeout)
		counter += 1;

		//On arrete le robot en cas de cible atteinte, de pause, d'arret d'urgence ou de timeout
		bool shouldStop = __mvtType != ROTATETO && dist < c->dist_closeEnoughTranslation;
		shouldStop |= __mvtType == ROTATETO && fabs(angleDist) < c->dist_closeEnoughRotation;
		shouldStop |= __mvtState == PAUSED;
		shouldStop |= __mvtState == ABORTED;
		didTimeout = (unsigned int) (counter * c->controlPeriod) > c->timeout;
		shouldStop |= didTimeout;

		if (shouldStop) {
			v_right = 0;
			v_left = 0;
			p_right -= (int)limit(p_right, c->maxPowerDerivative * c->controlPeriod);
			p_left -= (int)limit(p_left, c->maxPowerDerivative * c->controlPeriod);

			if (posRight == old_posRight && posLeft == old_posLeft && __mvtState != PAUSED) {
				p_right = 0;
				p_left = 0;
				__mvtState = NOMVT;

				//Attention : il faut attendre un petit temps avant de lancer un autre mouvement ;
				//sinon, __mvtState peut etre remodifie avant la fin de la boucle while.
				//S'ensuivent deux asservissements simultanes et donc un gros bazar.
			}
		}
		else {

			//Asservissement en position : obtention de la vitesse de consigne voulue
			if (false) { //mode de test d'asservissement en vitesse
				//v_rot = 0.1; //*sin((float)time1[T1]/1000/2*PI/0.5);
				v_str = 0.2; //*sin((float)time1[T1]/1000/2*PI/0.5);
			}
			else if (__mvtType == ROTATETO) {
				v_str = limit(v_str + limit(c->KPPosTranslation * dist_adapted - v_str, c->maxAccelTranslation * c->controlPeriod), c->maxSpeedTranslation);
				v_rot = limit(v_rot + limit(c->KPPosRotation * angleDist + c->KIPosRotation * i_angleDist - v_rot, c->maxAccelRotation * c->controlPeriod), c->maxSpeedRotation);
			}
			else {
				v_str = limit(v_str + limit(c->KPPosTranslation * dist_adapted + c->KIPosTranslation * i_dist_adapted - v_str, c->maxAccelTranslation * c->controlPeriod), c->maxSpeedTranslation);
				v_rot = limit(v_rot + limit(c->KPPosRotation*c->betweenWheels/2 * oriDiff_adapted + c->KIPosRotation*c->betweenWheels/2 * i_oriDiff_adapted - v_rot, c->maxAccelRotation * c->controlPeriod), c->maxSpeedRotation);
			}

			//Mise a jour des variables de commande
			//Mise a jour des variables integrees (partie 1)
			i_vDiff_right += vDiff_right * c->controlPeriod / 2;
			i_vDiff_left += vDiff_left * c->controlPeriod / 2;

			//Mise a jour des variables instantanees
			v_right = v_str + v_rot;
			v_left = v_str - v_rot;
			vDiff_right = v_right - (posRight - old_posRight) / c->controlPeriod;
			vDiff_left = v_left - (posLeft - old_posLeft) / c->controlPeriod;

			//Mise a jour des variables integrees (partie 2)
			i_vDiff_right += vDiff_right * c->controlPeriod / 2;
			i_vDiff_left += vDiff_left * c->controlPeriod / 2;

			//Asservissement en vitesse : obtention de la vitesse de consigne reelle
			p_right = (int)limit(p_right + limit(c->KPVitTranslation * vDiff_right + c->KIVitTranslation * i_vDiff_right - p_right, c->maxPowerDerivative * c->controlPeriod), c->maxPower);
			p_left = (int)limit(p_left + limit(c->KPVitTranslation * vDiff_left + c->KIVitTranslation * i_vDiff_left - p_left, c->maxPowerDerivative * c->controlPeriod), c->maxPower);

		}

		//Envoi de la commande aux moteurs
		motor[motorRight] = (c->rightMotorReversed ? -1 : 1) * p_right;
		motor[motorLeft] = (c->leftMotorReversed ? -1 : 1) * p_left;

		// affichage des logs lors des tests du correcteur
		datalogDataGroupStart();
		datalogAddValue(0, 100*v_str);
		datalogAddValue(1, 100*( (posRight - old_posRight)/c->controlPeriod + (posLeft - old_posLeft)/c->controlPeriod )/2);
		datalogDataGroupEnd();

		//Mise a jour des anciennes variables
		old_posRight = posRight;
		old_posLeft = posLeft;

	} while (__mvtState != NOMVT && __mvtState != PAUSED);

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

void resumeMovement(){
	if (__mvtState == PAUSED) {
		__mvtState = ONGOING;
		startTask(controlMovement);
	}
}

#endif
