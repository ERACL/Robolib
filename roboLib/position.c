#ifndef POSITION_H
#define POSITION_H

#include "config.c"

// ---------- CONSTANTES A MODIFIER ----------

const unsigned int positionUpdatePeriod = 20; //En ms.

// ---------- FIN DES CONSTANTES A MODIFIER ----------

//Structure de donnees de position.
//Distances en mm, angles en degres.
struct PosData {
	float x;
	float y;
	float orientation;
}

void initPosition(bool isGreenSide);

//Renvoie un posData constenant les donnees de position du robot, en mm et degres.
void getPosition(struct PosData const** data);

//Meme chose en ticks d'encode et radians, plus rapide car sans conversion.
void getRawPosition(struct PosData const** data);


//-- DEBUT DES DEFINITIONS --

PosData __position; //Contient en tout temps la position du robot (regulierement mise a jour) en tics d'encodage.

PosData __pos_standardized; //Contient la position convertie en mm, ne pas utiliser ; mis a jour dans getPosition() uniquement.

float __oldEncoderL;
float __oldEncoderR;
float __betweenWheelsInEncode;

void getPosition(struct PosData const** data) {
	struct Config const* c = NULL;
	getConfig(&c);
  __pos_standardized.x = __position.x * c->mmPerEncode;
  __pos_standardized.y = __position.y * c->mmPerEncode;
  __pos_standardized.orientation = __position.orientation * 180 / PI;
  while (__pos_standardized.orientation > 360)
  	__pos_standardized.orientation -= 360;
 	while (__pos_standardized.orientation < 0)
 		__pos_standardized.orientation += 360;
  *data = &__pos_standardized;
}

void getRawPosition(struct PosData const** data) {
	*data = &__position;
}

task updatePosition() {
  while (true) {
    wait1Msec(positionUpdatePeriod);
    float deltaL = nMotorEncoder[motorLeft] - __oldEncoderL;
    __oldEncoderL = nMotorEncoder[motorLeft];
    float deltaR = nMotorEncoder[motorRight] - __oldEncoderR;
    __oldEncoderR = nMotorEncoder[motorRight];
    float deltaD = (deltaR + deltaL) / 2;
    float deltaO = (deltaR - deltaL) / __betweenWheelsInEncode;
    float midO = __position.orientation + deltaO / 2;
    __position.x += cos(midO) * deltaD;
    __position.y += sin(midO) * deltaD;
    __position.orientation += deltaO;
  }
}

void initPosition(bool isGreenSide) {
	struct Config const* c = NULL;
	getConfig(&c);

  if (isGreenSide) {
    __position.x = c->initialX_GreenSide;
    __position.y = c->initialY_GreenSide;
    __position.orientation = c->initialOrientation_GreenSide * PI / 180;
  }
  else {
    __position.x = c->initialX_OrangeSide;
    __position.y = c->initialY_OrangeSide;
    __position.orientation = c->initialOrientation_OrangeSide * PI / 180;
  }
  nMotorEncoder[motorLeft] = 0;
  nMotorEncoder[motorRight] = 0;
  __oldEncoderL = 0;
  __oldEncoderR = 0;
  __betweenWheelsInEncode = c->betweenWheels / c->mmPerEncode;
  startTask(updatePosition);
}

#endif
