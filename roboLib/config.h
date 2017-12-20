#ifndef CONFIG_H
#define CONFIG_H

/* Configurations des constantes des robots.
 * Dans la struct Config sont recensees toutes les constantes dependantes du robot (notamment les constantes geometriques.
 * Elles sont a remplir dans config.c pour chaque robot. La configuration est chargee en debut de programme avec initCo
 * Les constantes sont ensuite obtenues dans tout le programme via getConfig().
 * Exemple : getConfig().mmPerEncode donne le nombre de mm par unite de l'encodeur incremental pour le robot choisi avec initConfig.
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
};

enum Robot {
  TESTROBOT, ERACL1_SMALL, ERACL1_BIG, ERACL2_SMALL, ERACL2_BIG
};

void initConfig(enum Robot robot); //A effectuer en TOUT DEBUT DE PROGRAMME, avant les autres initialisations (initPosition()...), qui en dependent.

void getConfig(struct Config const** c);

/* getConfig() devrait renvoyer un pointeur vers une struct const, mais c'est impossible avec le compilateur de robotC, qui a visiblement ete code par un 
 * Il faut donc passer en argument un pointeur vers ce pointeur !
 * De plus robotC croit que le pointeur vers la struct est lui-meme const, ce qui necessite de l'initialiser avec NULL sous peine d'erreur de compilation
 * Cette syntaxe se retrouvera dans toutes les fonctions de la bibliotheque qui renvoient des structures
 *
 * Exemple d'utilisation :
 *
 * struct Config const* c = NULL;
 * getConfig(&c);
 * float a = c->betweenWheels;
 */


/* On rajoute le code normalement dans les .c a la fin des .h, devinez pourquoi
 * ...
 * Si, si...
 * RobotC.
 * En consequence, toute variable normalement limitee au .c non-const suivront la syntaxe __nomDeVariable
 */


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
  		break;
  	}
    case ERACL1_SMALL: {
      break;
    }
    case ERACL1_BIG: {
      break;
    }
    case ERACL2_SMALL: {
      break;
    }
    case ERACL2_BIG: {
      break;
    }
  }
}

void getConfig(struct Config const** c) {
	*c = &__config;
}

#endif
