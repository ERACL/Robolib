#ifndef SONAR_C
#define SONAR_C

#include "config.c"
#include "movement.c"

// Les deux fonctions suivantes permettent d'obtenir la distance (en mm) entre le sonar avant (respectivement arriere) du robot et l'obstacle detecte.

task avoidObstacles() {
	struct Config const* c = NULL;
	getConfig(&c);

	while (true) {
		wait1Msec(20);
#ifdef sensorFront
		bool obstacleFront = SensorValue[sensorFront] * 10 < c->securityDistance;
#else
		bool obstacleFront = false;
#endif
#ifdef sensorBack
		bool obstacleBack = SensorValue[sensorBack] * 10 < c->securityDistance;
#else
		bool obstacleBack = false;
#endif
		if ((getMovementType() == MOVETO && (obstacleFront || obstacleBack))
				|| (getMovementType() == MOVETO_FORWARD && obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARD && obstacleBack))
			{ pauseMovement(); }
		else if ((!obstacleFront && !obstacleBack)
			  || (getMovementType() == MOVETO_FORWARD && !obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARD && !obstacleBack))
			{ resumeMovement(); }
	}
}

#endif
