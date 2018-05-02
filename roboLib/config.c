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

  //Constantes d'asservissement
  float controlPeriod; //ms

	float KPStr; //pow / mm : Coefficient proportionnel du correcteur de vitesse droite
	float KIStr; //pow / (mm * ms) : Coefficient integrateur du correcteur de vitesse droite
	float KPRot; //pow / rad : Coefficient proportionnel du correcteur de vitesse de rotation
	float KIRot; //pow / (rad * ms) : Coefficient integrateur du correcteur de vitesse de rotation

	float maxAccel; //mm/ms/ms
	float maxSpeed; //mm/ms

	float powerPerEPms; //pow / (enc / ms) : Puissance de consigne necessaire pour une vitesse de 1 encode/ms (constante moteur)

	float dist_close; //mm : Distance a partir de laquelle s'enclenche les integrations
	float dist_closeEnough; //mm : Distance consideree comme OK
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

  		__config.leftMotorReversed = false;
 			__config.rightMotorReversed = false;
  		__config.controlPeriod = 20;
			__config.KPStr = 0.13;
			__config.KIStr = 0.0002;
			__config.KPRot = 25;
			__config.KIRot = 0.12;
			__config.maxAccel = 0.0013;
			__config.maxSpeed = 0.3;
			__config.powerPerEPms = 100;
			__config.dist_close = 50;
			__config.dist_closeEnough = 3;
  		break;
    }
    case OBELIX: {
      __config.betweenWheels = 255;
  		__config.mmPerEncode = 0.40249;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.sensorFront = true;
  		__config.sensorBack = true;

  		__config.leftMotorReversed = true;
 			__config.rightMotorReversed = false;
  		__config.controlPeriod = 20;
			__config.KPStr = 0.13;
			__config.KIStr = 0.0002;
			__config.KPRot = 25;
			__config.KIRot = 0.12;
			__config.maxAccel = 0.0013;
			__config.maxSpeed = 0.3;
			__config.powerPerEPms = 50;
			__config.dist_close = 50;
			__config.dist_closeEnough = 3;
  		break;
    }
    case TULLIUS: {
    	__config.betweenWheels = 212;
  		__config.mmPerEncode = 0.634;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.initialOrientation_OrangeSide = 180;
  		__config.initialX_OrangeSide = 0;
  		__config.initialY_OrangeSide = 0;
  		__config.sensorFront = true;
  		__config.sensorBack = true;
  		__config.securityDistance = 200;

  		__config.leftMotorReversed = false;
 			__config.rightMotorReversed = false;
		  __config.controlPeriod = 20;
			__config.KPStr = 0.13;
			__config.KIStr = 0.0002;
			__config.KPRot = 25;
			__config.KIRot = 0.12;
			__config.maxAccel = 0.001;
			__config.maxSpeed = 0.25;
			__config.powerPerEPms = 100;
			__config.dist_close = 50;
			__config.dist_closeEnough = 3;
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
