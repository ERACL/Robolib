#ifndef SONAR_C
#define SONAR_C

#include "config.c"
#include "movement.c"

task avoidObstacles() {
	struct Config const* c = NULL;
	getConfig(&c);

	while (doAvoidObstacles) {
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
		if ((getMovementType() == MOVETO && obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARDS && obstacleBack))
			{ pauseMovement(); displayBigTextLine(7, "obstacle"); }
		else if ((!obstacleFront && !obstacleBack)
			  || (getMovementType() == MOVETO && !obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARDS && !obstacleBack))
			{ resumeMovement(); displayBigTextLine(7, "OK");}
	}
}

#endif
