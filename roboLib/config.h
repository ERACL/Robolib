#ifndef CONFIG_H
#define CONFIG_H

/* Configurations des constantes des robots.
 * Dans la struct Config sont recensees toutes les constantes dependant du robot (notamment les constantes geometriques).
 * Elles sont a remplir dans la partie definition pour chaque robot. La configuration est chargee en debut de programme avec initConfig.
 * Les constantes sont ensuite obtenues dans tout le programme via getConfig().
 */

struct Config {
  float betweenWheels; //136 mm
  float mmPerEncode; //757.12 tics par tour sur moteur MATRIX
  float initialX_GreenSide;
  float initialX_OrangeSide;
  float initialY_GreenSide;
  float initialY_OrangeSide;
  float initialOrientation_GreenSide;
  float initialOrientation_OrangeSide;
  bool sensorFront;
  bool sensorBack;

  //Constantes utilisees dans l'asservissement
  float maxAcceleration; //Acceleration maximale autorisee (pour eviter les glissements)
  float K_m; //Gain du moteur
  float tau_m //Temps caracteristique du moteur (1er ordre)
  float K_p; //Gain de la partie proportionnelle du correcteur
  float K_i; //Gain de la partie integrateur du correcteur
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
  		__config.sensorFront = 0;
  		__config.sensorBack = 0;
  	}
    	case JIMMY: {
	      	__config.betweenWheels = 150;
  		__config.mmPerEncode = 0.51487;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.sensorFront = 1;
  		__config.sensorBack = 0;
  		break;
    	}
    	case OBELIX: {
      	      	__config.betweenWheels = 230;
  		__config.mmPerEncode = 0.40249;
  		__config.initialOrientation_GreenSide = 0;
  		__config.initialX_GreenSide = 0;
  		__config.initialY_GreenSide = 0;
  		__config.sensorFront = 1;
  		__config.sensorBack = 1;
  		break;
    	}
    case TULLIUS: {
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
