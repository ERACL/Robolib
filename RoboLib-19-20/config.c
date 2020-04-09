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

	float initialX_BlueSide; //mm
	float initialX_YellowSide;
	float initialY_BlueSide;
	float initialY_YellowSide;
	float initialOrientation_BlueSide; //degres
	float initialOrientation_YellowSide;


	bool leftMotorReversed;
	bool rightMotorReversed;

	float securityDistance; //mm
	unsigned int timeout; //ms

	float maxPower; //pow
	float maxPowerDerivative; //pow / ms

	unsigned int controlPeriod; //ms

	//Constantes d'asservissement en translation

	//Asservissement en position : position -> vitesse voulue
	float KPPosTranslation; //Coefficient proportionnel : (mm / ms) / mm = ms^-1
	float KIPosTranslation; //Coefficient integrateur : (mm / ms) / (mm * ms) = ms^-2
	float integDistTranslation; //Distance a la cible a partir de laquelle l'integrateur se lance (mm)
	float maxEffectiveDistTranslation; //Distance maximale utilisee dans l'asservissement, pour limiter les consignes a tres grande distance

	//Asservissement en vitesse : vitesse voulue -> puissance envoyée
	float KPVitTranslation; //Coefficient proportionnel : pow / (mm / ms)
	float KIVitTranslation; //Coefficient integrateur : pow / (mm / ms * ms) = pow / mm

	float maxSpeedTranslation; //mm / ms
	float maxAccelTranslation; //mm / ms^2

	float dist_closeEnoughTranslation; //Precision toleree en distance a la cible : mm

	//Distance a la cible a partir de laquelle le robot est autorise a aller en arriere ; -1 pour infini.
	//Eviter 0, sinon tout depassement devient un cauchemar (le robot doit se retourner pour revenir sur ses pas)
	//Attention, dans la zone autorisee il n'y a plus de difference entre moveTo, moveTo_forward et moveTo_backward.
	float dist_allowBackwardTranslation;



	//Constantes d'asservissement en translation

	//Asservissement en position : position -> vitesse voulue
	float KPPosRotation; //Coefficient proportionnel : (mm / ms) / mm = ms^-1
	float KIPosRotation; //Coefficient integrateur : (mm / ms) / (mm * ms) = ms^-2
	float integDistRotation; //Distance a la cible a partir de laquelle l'integrateur se lance (mm)
	float maxEffectiveDistRotation; //Distance maximale utilisee dans l'asservissement, pour limiter les consignes a tres grande distance

	//Asservissement en vitesse : vitesse voulue -> puissance envoyée
	float KPVitRotation; //Coefficient proportionnel : pow / (mm / ms)
	float KIVitRotation; //Coefficient integrateur : pow / (mm / ms * ms) = pow / mm

	float maxSpeedRotation; //mm / ms
	float maxAccelRotation; //mm / ms^2

	float dist_closeEnoughRotation; //Precision toleree en distance a la cible : mm

	//Distance a la cible a partir de laquelle le robot est autorise a aller en arriere ; -1 pour infini.
	//Eviter 0, sinon tout depassement devient un cauchemar (le robot doit se retourner pour revenir sur ses pas)
	//Attention, dans la zone autorisee il n'y a plus de difference entre moveTo, moveTo_forward et moveTo_backward.
	float dist_allowBackwardRotation;
};

enum Robot {
	OBELIX, IDEFIX
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
		case IDEFIX: {
			__config.betweenWheels = 174.5;
			__config.mmPerEncode = 0.63;
			__config.initialOrientation_BlueSide = 0;
			__config.initialX_BlueSide = 0;
			__config.initialY_BlueSide = 0;
			__config.initialOrientation_YellowSide = 0;
			__config.initialX_YellowSide = 0;
			__config.initialY_YellowSide = 0;
			__config.securityDistance = 200;

			__config.timeout = 7000;

			__config.leftMotorReversed = true;
			__config.rightMotorReversed = true;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 10;
			__config.controlPeriod = 30;

			__config.dist_closeEnoughTranslation = 5;

			__config.KPPosTranslation = 0.0045; //0.004;
			__config.KIPosTranslation = 0.00000005; //0.00000005;
			__config.integDistTranslation = 100;
			__config.maxEffectiveDistTranslation = 100;
			__config.maxSpeedTranslation = 0.1; //0.4;
			__config.maxAccelTranslation = 0.002;

			__config.dist_allowBackwardTranslation = 50;

			__config.KPVitTranslation = 200;
			__config.KIVitTranslation = 1;


			__config.dist_closeEnoughRotation = 1;

			__config.KPPosRotation = 0.0045; //0.004;
			__config.KIPosRotation = 0.00000005; //0.00000005;
			__config.integDistRotation = 100;
			__config.maxEffectiveDistRotation = 100;
			__config.maxSpeedRotation = 0.1; //0.4;
			__config.maxAccelRotation = 0.002;

			__config.dist_allowBackwardRotation = 50;

			__config.KPVitRotation = 200;
			__config.KIVitRotation = 1;
			break;
		}
		case OBELIX: {
			__config.betweenWheels = 160;
			__config.mmPerEncode = 0.525;

  		// Yellow : à droite si on a les manches à air devant
			__config.initialOrientation_BlueSide = 0;
			__config.initialX_BlueSide = 30;
			__config.initialY_BlueSide = 1100;
			__config.initialOrientation_YellowSide = 180;
			__config.initialX_YellowSide = 2970;
			__config.initialY_YellowSide = 1100;
			__config.securityDistance = 200;

			__config.timeout = 7000;

			__config.leftMotorReversed = false;
			__config.rightMotorReversed = false;
			__config.maxPower = 80;
			__config.maxPowerDerivative = 10;
			__config.controlPeriod = 30;


			__config.dist_closeEnoughTranslation = 5;

			__config.KPPosTranslation = 0.004; //0.004;
			__config.KIPosTranslation = 0.00000005; //0.00000005;
			__config.integDistTranslation = 100;
			__config.maxEffectiveDistTranslation = 100;
			__config.maxSpeedTranslation = 0.25; //0.4;
			__config.maxAccelTranslation = 0.002;

			__config.dist_allowBackwardTranslation = 50;

			__config.KPVitTranslation = 200;
			__config.KIVitTranslation = 1;


			__config.dist_closeEnoughRotation = 1;

			__config.KPPosRotation = 0.0025; //0.004;
			__config.KIPosRotation = 0.00000001; //0.00000005;
			__config.integDistRotation = 100;
			__config.maxEffectiveDistRotation = 100;
			__config.maxSpeedRotation = 0.15; //0.4;
			__config.maxAccelRotation = 0.0005; // 0.001

			__config.dist_allowBackwardRotation = 50;

			__config.KPVitRotation = 200;
			__config.KIVitRotation = 1;
			break;
		}
	}
}

void getConfig(struct Config const** c) {
	*c = &__config;

	struct Config conf;
	**c = conf;
}

#endif
