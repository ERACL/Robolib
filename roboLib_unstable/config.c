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

	//Asservissement en vitesse : vitesse voulue -> puissance envoyée
	float KPVit; //Coefficient proportionnel : pow / (mm / ms)
	float KIVit; //Coefficient integrateur : pow / (mm / ms * ms) = pow / mm

	float maxSpeed; //mm / ms
	float maxAccel; //mm / ms^2
	float maxPower; //pow
	float maxPowerDerivative; //pow / ms

	unsigned int anglePriorityFactor; //Gere la preponderance du mouvement de rotation sur celui d'avance droite
	float rotateAttenuationCoeff; //Attenue la rotation dans les moveTo quand le robot est proche de la cible ; 0 = pas d'atténuation

	float dist_closeEnough; //Precision toleree en distance a la cible : mm

	//Distance a la cible a partir de laquelle le robot est autorise a aller en arriere ; -1 pour infini.
	//Eviter 0, sinon tout depassement devient un cauchemar (le robot doit se retourner pour revenir sur ses pas)
	//Attention, dans la zone autorisee il n'y a plus de difference entre moveTo, moveTo_forward et moveTo_backward.
	float dist_allowBackward;
};

enum Robot {
	JIMMY, OBELIX, TULLIUS, ROCCO
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
	case JIMMY: {
			__config.betweenWheels = 154.7;
			__config.mmPerEncode = 0.53;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.sensorFront = true;
			__config.sensorBack = false;

			__config.timeout = 10000;

			__config.leftMotorReversed = false;
			__config.rightMotorReversed = false;
			__config.controlPeriod = 30;
			__config.dist_closeEnough = 5;

			__config.KPPos = 0.005;
			__config.KIPos = 0.00003;
			__config.integDist = 20;
			__config.maxSpeed = 0.5;
			__config.maxAccel = 0.002;

			__config.anglePriorityFactor = 3;
			__config.rotateAttenuationCoeff = 5;
			__config.dist_allowBackward = 50;

			__config.KPVit = 100;
			__config.KIVit = 0.03;
			__config.maxPower = 70;
			__config.maxPowerDerivative = 0.15;
			break;
		}
	case OBELIX: {
			__config.betweenWheels = 250;
			__config.mmPerEncode = 0.408;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.initialOrientation_OrangeSide = 0;
			__config.initialX_OrangeSide = 0;
			__config.initialY_OrangeSide = 0;
			__config.sensorFront = false;
			__config.sensorBack = false;

			__config.timeout = 1000000;

			__config.leftMotorReversed = true;
			__config.rightMotorReversed = false;
			__config.controlPeriod = 5;
			__config.dist_closeEnough = 5;

			__config.KPPos = 0.01;
			__config.KIPos = 0.00007;
			__config.integDist = 40;
			__config.maxSpeed = 0.7;
			__config.maxAccel = 0.005;

			__config.anglePriorityFactor = 20;
			__config.rotateAttenuationCoeff = 5;
			__config.dist_allowBackward = 50;

			__config.KPVit = 70;
			__config.KIVit = 0.04;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 0.5;
			break;
		}
	case TULLIUS: {
			__config.betweenWheels = 206;
			__config.mmPerEncode = 0.625;
			__config.initialOrientation_GreenSide = 0;
			__config.initialX_GreenSide = 0;
			__config.initialY_GreenSide = 0;
			__config.initialOrientation_OrangeSide = 180;
			__config.initialX_OrangeSide = 0;
			__config.initialY_OrangeSide = 0;
			__config.sensorFront = false;
			__config.sensorBack = false;
			__config.securityDistance = 230;

			__config.timeout = 10000;

			__config.leftMotorReversed = false;
			__config.rightMotorReversed = false;
			__config.controlPeriod = 30;
			__config.dist_closeEnough = 5;

			__config.KPPos = 0.002;
			__config.KIPos = 0;
			__config.integDist = 15;
			__config.maxSpeed = 0.4;
			__config.maxAccel = 0.0015;

			__config.anglePriorityFactor = 3;
			__config.rotateAttenuationCoeff = 5;
			__config.dist_allowBackward = 50;

			__config.KPVit = 140;
			__config.KIVit = 1.3;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 10;
			break;
		}
	case ROCCO: {
			break;
		}
	}
}

void getConfig(struct Config const** c) {
	*c = &__config;
}

#endif
