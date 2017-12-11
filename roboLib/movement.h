#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "config.h"
#include "position.h"

/* Biblioth�que de mouvement haut-niveau et bas niveau.
 * La partie bas niveau effectue des d�placements en s'arr�tant en cas d'obstacles, et g�re l'asservissement des moteurs.
 * La partie haut niveau permet au robot d'aller d'un point A � un point B en g�rant les obstacles de fa�on optimale.
 */

//Partie haut niveau

const float timeout = 10000;

/* moveTo() trouve les mouvements individuels optimaux pour aller jusqu'� la destination.
 * les obstacles sont g�r�s (et le mouvement retent�) tant que le mouvement n'a pas exc�d� la valeur de timeout, auquel cas false sera renvoy�.
 * si ignoreOrientation est vrai, le robot ne cherchera pas � se mettre dans l'axe sp�cifi� dans target.
 */
bool moveTo(struct PosData const* target, bool ignoreOrientation = false);

//moveTo pr�f�re utiliser arc() pour �conomiser des mouvements ; ici, des straight() seront utilis�s.
bool moveTo_forceStraight(struct PosData const* target, bool ignoreOrientation = false);

//Partie bas-niveau

/* MovementStates recense les diff�rents �tats possibles d'un mouvement :
 * TBD : mouvement � faire, non commenc� ;
 * ONGOING : mouvement en cours d'ex�cution ;
 * DONE : mouvement correctement effectu� ;
 * FAILED_OBS : mouvement �chou� du fait d'un obstacle rencontr� ;
 * FAILED_OTHER : mouvement �chou� pour une autre raison (batterie faible, etc.).
 * Les �tats TBD et ONGOING ne devraient normalement pas �tre observ�s, mais peuvent �tre utiles pour du d�buggage.
 */
enum MovementState {
	TBD, ONGOING, DONE, FAILED_OBS, FAILED_OTHER
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

void arc(float distance, float angle); //La distance est la distance entre le point de d�part et le point d'arriv�e (pas la distance parcourure).

void rotate(float angle);

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

//Angle maximal pour lequel le robot choisit d'effectuer un arc de cercle plut�t que de tourner sur soi-m�me avant, en degr�s, durant l'ex�cution de moveTo().
const float maxAngleForArc = 35;

// ---------- FIN DES CONSTANTES A MODIFIER ----------

struct MovementResult __result;

//TODO : g�rer les �checs et le timeout.
bool moveTo(struct PosData const* target, bool ignoreOrientation) {
	struct PosData const* pos = NULL;
	getPosition(&pos);

	float targetDirection = atan((target->y - pos->y) / (target->x - pos->x)) * 180 / PI;
	float deltaOToFace = targetDirection - pos->orientation; //Diff�rence entre l'orientation que le robot aurait si dirig� vers la cible et son orientation actuelle.
	float deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));

	if (deltaOToFace < maxAngleForArc) { //On �conomise une rotation avant le mouvement vers la cible.
		arc(deltaD, deltaOToFace);
		getPosition(&pos);
		if (!ignoreOrientation)
			rotate(target->orientation - pos->orientation);
	}
	else if (!ignoreOrientation && target->orientation - targetDirection < maxAngleForArc) { //On �conomise une rotation apr�s le mouvement vers la cible.
		rotate(target->orientation + 2 * (targetDirection - target->orientation));
		getPosition(&pos); //On actualise la position et les deltas, m�me si deltaD ne devrait pas avoir chang�, au cas o�.
		deltaOToFace = targetDirection - pos->orientation;
		deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
		arc(deltaD, deltaOToFace);
	}
	else { //On effectue les trois mouvements sans conditions exceptionnelles (deux si ignoreOrientation est vrai).
		rotate(deltaOToFace);
		getPosition(&pos);
		deltaOToFace = targetDirection - pos->orientation;
		deltaD = sqrt(pow(target->x - pos->x, 2) + pow(target->y - pos->y, 2));
		arc(deltaD, deltaOToFace); //On effectue tout de m�me un arc, au cas o�.
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
