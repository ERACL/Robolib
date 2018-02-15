#pragma config(Sensor, S3,     redButton,      sensorEV3_Touch)

#include "config.c"
#include "movement.c"

task stopRedButton(){
	if (SensorValue(redButton) == 1){}
	wait1Msec(20);
}
