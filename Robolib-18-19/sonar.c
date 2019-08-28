#ifndef SONAR_C
#define SONAR_C

#include "config.c"
#include "movement.c"

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
#ifdef sensorFront2
		obstacleFront |= SensorValue[sensorFront2] * 10 < c->securityDistance;
#endif
#ifdef sensorBack
		bool obstacleBack = SensorValue[sensorBack] * 10 < c->securityDistance;
#else
		bool obstacleBack = false;
#endif
		if ((getMovementType() == MOVETO_FORWARD && obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARD && obstacleBack))
			{ pauseMovement(); displayTextLine(7, "obstacle"); }
		else if ((!obstacleFront && !obstacleBack)
			  || (getMovementType() == MOVETO_FORWARD && !obstacleFront)
				|| (getMovementType() == MOVETO_BACKWARD && !obstacleBack))
			{ resumeMovement(); displayTextLine(7, "OK");}
	}
}

#endif
