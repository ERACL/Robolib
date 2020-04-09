#ifndef SONAR_C
#define SONAR_C

#include "config.c"
#include "movement.c"


// definition des variables s'il n'y a pas de capteurs indique
#ifndef sensorFront
#define sensorFront 0
#endif
#ifndef sensorFront2
#define sensorFront2 0
#endif
#ifndef sensorBack
#define sensorBack 0
#endif


// renvoie true si le point (x,y) se trouve dans le terrain (3m (x) * 2m (y))
// x, y en mm
bool isInField(float x, float y){
	return x >= 0 && x <= 3000 && y >= 0 && y <= 2000;
}


task avoidObstacles() {
	struct Config const* c = NULL;
	getConfig(&c);

	while (true) {
		wait1Msec(20);

/* Version 2018-2019:
#ifdef sensorFront
		bool obstacleFront = SensorValue[sensorFront] * 10 < c->securityDistance;
#else
		bool obstacleFront = false;
#endif
#ifdef sensorFront2
		obstacleFront |= SensorValue[sensorFront2] * 10 < c->securityDistance;
#endif
#ifdef sensorBack
		bool obstacleBack = SensorValue[sensorBack] * 10 < c->securityDistance;
#else
		bool obstacleBack = false;
#endif
*/

		bool obstacleFront = false;
		bool obstacleBack = false;

		struct PosData pos;
		getPosition(&pos);

		// Regarde, lorsqu'un capteur est indique, si la distance est inférieure a la distance de securite,
		// et si l'objet rencontre est a l'interieur du terrain
		if(sensorFront != 0){
			struct PosData posObstacleFront = pos;
			float value = SensorValue[sensorFront];
			posObstacleFront.x += value*10*cosDegrees(pos.orientation);
			posObstacleFront.y += value*10*sinDegrees(pos.orientation);
			obstacleFront = isInField(posObstacleFront.x, posObstacleFront.y) && value * 10 < c->securityDistance;
		}
		if(sensorFront2 != 0){
			struct PosData posObstacleFront = pos;
			float value = SensorValue[sensorFront2];
			posObstacleFront.x += value*10*cosDegrees(pos.orientation);
			posObstacleFront.y += value*10*sinDegrees(pos.orientation);
			obstacleFront |= isInField(posObstacleFront.x, posObstacleFront.y) && value * 10 < c->securityDistance;
		}
		if(sensorBack != 0){
			struct PosData posObstacleBack = pos;
			float value = SensorValue[sensorBack];
			posObstacleBack.x += value*10*cosDegrees(pos.orientation);
			posObstacleBack.y += value*10*sinDegrees(pos.orientation);
			obstacleBack = isInField(posObstacleBack.x, posObstacleBack.y) && value * 10 < c->securityDistance;
		}

		if ((getMovementType() == MOVETO_FORWARD && obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARD && obstacleBack))
			{
				pauseMovement();
				displayTextLine(7, "obstacle");
			}
		else if ((!obstacleFront && !obstacleBack)
			  || (getMovementType() == MOVETO_FORWARD && !obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARD && !obstacleBack))
			{
				resumeMovement();
				displayTextLine(7, "No obstacle");
			}
	}
}

#endif
