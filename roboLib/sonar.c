#ifndef SONAR_C
#define SONAR_C

#include "config.c"
#include "movement.c"

// Les deux fonctions suivantes permettent d'obtenir la distance (en mm) entre le sonar avant (respectivement arriere) du robot et l'obstacle detecte.

task avoidObstacles() {
	struct Config const* c = NULL;
	getConfig(&c);

	while (true) {
		wait1Msec(50);
		bool obstacleFront = SensorValue[sensorFront] * 10 < c->securityDistance;
		bool obstacleBack = false;
		if ((getMovementType() == MOVETO && obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARDS && obstacleBack))
			{ pauseMovement(); }
		else if ((!obstacleFront && !obstacleBack)
			  || (getMovementType() == MOVETO && !obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARDS && !obstacleBack))
			{ resumeMovement(); }
	}
}

float getObstacleDistanceFront();
float getObstacleDistanceBack();


//-- DEBUT DES DEFINITIONS --


#endif
