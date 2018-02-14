#ifndef CONFIG_H
#define CONFIG_H

/* Configurations des constantes des robots.
 * Dans la struct Config sont recensees toutes les constantes dependant du robot (notamment les constantes geometriques).
 * Elles sont a remplir dans la partie definition pour chaque robot. La configuration est chargee en debut de programme avec initConfig.
 * Les constantes sont ensuite obtenues dans tout le programme via getConfig().
 * Notons que certaines constantes (temps d'asservissement...) sont indépendantes des robots et se trouvent donc dans leurs fichiers respectifs.
 */

struct Config {
  float betweenWheels;
  float mmPerEncode;
  float initialX_GreenSide;
  float initialX_OrangeSide;
  float initialY_GreenSide;
  float initialY_OrangeSide;
  float initialOrientation_GreenSide;
  float initialOrientation_OrangeSide;
  bool sensorFront;
  bool sensorBack;
  float securityDistance;

  //Constantes utilisees dans l'asservissement (distances en encodes, angles en radians pour raisons de performances)
  float K_m; //Gain du moteur : (encode / sec) / puissance
  float tau_m; //Temps caracteristique du moteur (1er ordre) : ms
  float K_p; //Gain de la partie proportionnelle du correcteur : puissance / encode
  float K_i; //Gain de la partie integrateur du correcteur : puissance / encode / ms
  float integralControlDistance; //Distance a la cible a laquelle le correcteur integral s'engage : encode
  float maxAllowedPower;
  float maxAllowedPowerDerivative; //Acceleration de consigne maximale authorisee : puissance / ms
	float errorMarginDistance; //Erreur de distance a la cible toleree : encode
	float errorMarginAngle; //Erreur d'angle a la cible toleree : radians
};

enum Robot {
  TESTROBOT, JIMMY, OBELIX, TULLIUS, ROCCO
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
  	case TESTROBOT: {
  		__config.betweenWheels = 230;
  		__config.mmPerEncode = 0.39834;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.sensorFront = false;
  		__config.sensorBack = false;
  		break;
  	}
    case JIMMY: {
	    __config.betweenWheels = 154.7;
  		__config.mmPerEncode = 0.53;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.sensorFront = true;
  		__config.sensorBack = false;
  		__config.K_m = 0.01;
  		__config.K_p = 0.2;
  		__config.K_i = 0.0005;
  		__config.tau_m = 140;
  		__config.maxAllowedPower = 45;
  		__config.maxAllowedPowerDerivative = 0.1;
  		__config.errorMarginDistance = 3 / __config.mmPerEncode;
  		__config.errorMarginAngle = PI / 180;
  		__config.integralControlDistance = 10 / __config.mmPerEncode;
  		break;
    }
    case OBELIX: {
      __config.betweenWheels = 230;
  		__config.mmPerEncode = 0.40249;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.sensorFront = true;
  		__config.sensorBack = true;
  		break;
    }
    case TULLIUS: {
    	__config.betweenWheels = 209.3;
  		__config.mmPerEncode = 0.634;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.initialOrientation_OrangeSide = 180;
  		__config.initialX_OrangeSide = 0;
  		__config.initialY_OrangeSide = 0;
  		__config.sensorFront = true;
  		__config.sensorBack = false;
  		__config.K_m = 0.01;
  		__config.K_p = 0.2;
  		__config.K_i = 0.0005;
  		__config.tau_m = 140;
  		__config.maxAllowedPower = 45;
  		__config.maxAllowedPowerDerivative = 0.1;
  		__config.errorMarginDistance = 3 / __config.mmPerEncode;
  		__config.errorMarginAngle = PI / 180;
  		__config.integralControlDistance = 10 / __config.mmPerEncode;
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
