#ifndef CONFIG_H
#define CONFIG_H

/* Configurations des constantes des robots.
* Dans la struct Config sont recensees toutes les constantes dependant du robot (notamment les constantes geometriques).
* Elles sont a remplir dans la partie definition pour chaque robot. La configuration est chargee en debut de programme avec initConfig.
* Les constantes sont ensuite obtenues dans tout le programme via getConfig().
*/

struct Config {
	float betweenWheels; //mm
	float mmPerEncode;

	float initialX_GreenSide; //mm
	float initialX_OrangeSide;
	float initialY_GreenSide;
	float initialY_OrangeSide;
	float initialOrientation_GreenSide; //degres
	float initialOrientation_OrangeSide;

	bool sensorFront;
	bool sensorBack;

	bool leftMotorReversed;
	bool rightMotorReversed;

	float securityDistance; //mm
	unsigned int timeout; //ms

	//Constantes d'asservissement
	unsigned int controlPeriod; //ms

	//Asservissement en position : position -> vitesse voulue
	float KPPos; //Coefficient proportionnel : (mm / ms) / mm = ms^-1
	float KIPos; //Coefficient integrateur : (mm / ms) / (mm * ms) = ms^-2
	float integDist; //Distance a la cible a partir de laquelle l'integrateur se lance (mm)
	float maxEffectiveDist; //Distance maximale utilisee dans l'asservissement, pour limiter les consignes a tres grande distance

	//Asservissement en vitesse : vitesse voulue -> puissance envoyée
	float KPVit; //Coefficient proportionnel : pow / (mm / ms)
	float KIVit; //Coefficient integrateur : pow / (mm / ms * ms) = pow / mm

	float maxSpeed; //mm / ms
	float maxAccel; //mm / ms^2
	float maxPower; //pow
	float maxPowerDerivative; //pow / ms

	float dist_closeEnough; //Precision toleree en distance a la cible : mm

	//Distance a la cible a partir de laquelle le robot est autorise a aller en arriere ; -1 pour infini.
	//Eviter 0, sinon tout depassement devient un cauchemar (le robot doit se retourner pour revenir sur ses pas)
	//Attention, dans la zone autorisee il n'y a plus de difference entre moveTo, moveTo_forward et moveTo_backward.
	float dist_allowBackward;
};

enum Robot {
	RANTANPLAN, LUCKY, JOE, AVERELL
};

void initConfig(enum Robot robot); //A effectuer en TOUT DEBUT DE PROGRAMME, avant les autres initialisations (initPosition()...), qui en dependent.

void getConfig(struct Config const** c);

/* getConfig() devrait renvoyer un pointeur vers une struct const, mais c'est impossible avec le compilateur de robotC, qui a visiblement ete code par des singes.
* Il faut donc passer en argument un pointeur vers ce pointeur !
* De plus robotC croit que le pointeur vers la struct est lui-meme const, ce qui necessite de l'initialiser avec NULL sous peine d'erreur de compilation !
* Cette syntaxe se retrouvera dans toutes les fonctions de la bibliotheque qui sont censees renvoyer des structures.
*
* Exemple d'utilisation :
*
* struct Config const* c = NULL;
* getConfig(&c);
* float a = c->betweenWheels;
*/


/* On rajoute le code normalement dans les .c a la fin des .h, devinez pourquoi.
* ...
* Si, si...
* RobotC.
* En consequence, toute variable normalement limitee au .c non-const suivront la syntaxe __nomDeVariable.
*/

//On notera enfin que l'absence de caracteres speciaux dans la bibliotheque est elle aussi due a RobotC.


//-- DEBUT DES DEFINITIONS --


struct Config __config;

//TODO : A remplir !
void initConfig(enum Robot robot) {
	switch (robot) {
		case RANTANPLAN: {
			__config.betweenWheels = 275;
			__config.mmPerEncode = 0.420;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.initialOrientation_OrangeSide = 0;
			__config.initialX_OrangeSide = 0;
			__config.initialY_OrangeSide = 0;
			__config.sensorFront = true;
			__config.sensorBack = true;
			__config.securityDistance = 200;

			__config.timeout = 100000;

			__config.leftMotorReversed = true;
			__config.rightMotorReversed = false;
			__config.controlPeriod =50; // 100;
			__config.dist_closeEnough = 5;

			__config.KPPos = 0.0045; //0.004;
			__config.KIPos = 0.000000000000000000001 //0.00000005;
			__config.integDist = 100;
			__config.maxEffectiveDist = 100;
			__config.maxSpeed = 0.4; //0.4;
			__config.maxAccel = 0.002;

			__config.dist_allowBackward = 50;

			__config.KPVit = 70;
			__config.KIVit = 0.14;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 0.3;
			break;
		}
		case LUCKY: {
			__config.betweenWheels = 255;
			__config.mmPerEncode = 0.420;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.initialOrientation_OrangeSide = 0;
			__config.initialX_OrangeSide = 0;
			__config.initialY_OrangeSide = 0;
			__config.sensorFront = true;
			__config.sensorBack = true;
			__config.securityDistance = 180; //200;

			__config.timeout = 90000;

			__config.leftMotorReversed = true;
			__config.rightMotorReversed = false;
			__config.controlPeriod = 100;
			__config.dist_closeEnough = 10;

			__config.KPPos = 0.0034;
			__config.KIPos = 0.0000002;
			__config.integDist = 40;
			__config.maxEffectiveDist = 100;
			__config.maxSpeed = 0.3;
			__config.maxAccel = 0.0015;

			__config.dist_allowBackward = 50;

			__config.KPVit = 50;
			__config.KIVit = 0.30;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 0.3;
			break;
			}
		case JOE: {
			__config.betweenWheels = 150;
			__config.mmPerEncode = 0.65;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.initialOrientation_OrangeSide = 0;
			__config.initialX_OrangeSide = 0;
			__config.initialY_OrangeSide = 0;
			__config.sensorFront = true;
			__config.sensorBack = true;
			__config.securityDistance = 200;

			__config.timeout = 7000;

			__config.leftMotorReversed = true;
			__config.rightMotorReversed = true;
			__config.controlPeriod = 30;
			__config.dist_closeEnough = 5;

			__config.KPPos = 0.002;
			__config.KIPos = 0.00001;
			__config.integDist = 15;
			__config.maxSpeed = 0.3;
			__config.maxAccel = 0.0015;
			__config.maxEffectiveDist = 100;

			__config.dist_allowBackward = 50;

			__config.KPVit = 200;
			__config.KIVit = 1;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 10;
			break;
		}
		case AVERELL: {
			__config.betweenWheels = 150;
			__config.mmPerEncode = 0.52;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.initialOrientation_OrangeSide = 0;
			__config.initialX_OrangeSide = 0;
			__config.initialY_OrangeSide = 0;
			__config.sensorFront = true;
			__config.sensorBack = false;
			__config.securityDistance = 100;

			__config.timeout = 7000;

			__config.leftMotorReversed = true;
			__config.rightMotorReversed = true;
			__config.controlPeriod = 30;
			__config.dist_closeEnough = 5;

			__config.KPPos = 0.005;
			__config.KIPos = 0.00001;
			__config.integDist = 5;
			__config.maxSpeed = 1;
			__config.maxAccel = 0.002;
			__config.maxEffectiveDist = 100;

			__config.dist_allowBackward = 50;

			__config.KPVit = 200;
			__config.KIVit = 1;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 10;
			break;
		}
	}
}

void getConfig(struct Config const** c) {
	*c = &__config;
}

#endif
