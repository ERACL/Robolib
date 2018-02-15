#pragma config(Sensor, S3,     redButton,      sensorEV3_Touch)

#include "config.c"
#include "movement.c"

// La task suivante verifie toutes les 20 ms si le bouton d'arret d'urgence a ete actionne.
task stopRedButton(){ 
	if (SensorValue(redButton) == 1){}
	wait1Msec(20);
}
