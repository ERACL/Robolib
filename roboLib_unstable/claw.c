#ifndef CLAW_C
#define CLAW_C

const int encodes_clawClosed = -180; //Nombre d'encodes au moteur de fermeture pour que la pince soit fermee, avec 0 le nombre a l'ouverture
const int encodes_clawPos1 = -240; //Idem, pos0 = 0
const int encodes_clawPos2 = -420;

void setClawPos(int pos);

void openClaw();

void closeClaw();

bool isClawMoving();

//-- DEBUT DES DEFINITIONS --

void setClawPos(int pos) {
	while (isClawMoving()) { wait1Msec(10); }
	if (pos <= 0)
		setMotorTarget(motorLiftClaw, 0, 30);
	else if (pos == 1)
		setMotorTarget(motorLiftClaw, encodes_clawPos1, 30);
	else
		setMotorTarget(motorLiftClaw, encodes_clawPos2, 30);
	wait1Msec(100);
}

void openClaw() {
	while (isClawMoving()) { wait1Msec(10); }
	setMotorTarget(motorOpenClaw, 0, 30);
	wait1Msec(100);
}

void closeClaw() {
	while (isClawMoving()) { wait1Msec(10); }
	setMotorTarget(motorOpenClaw, encodes_clawClosed, 30);
	wait1Msec(100);
}

bool isClawMoving() {
	return fabs(getMotorRPM(motorLiftClaw)) > 1 || fabs(getMotorRPM(motorOpenClaw)) > 1;
}

#endif
