float getMaxSpeed(int power) {
	motor[motorA] = power;
	motor[motorB] = power;
	wait1Msec(1000);
	nMotorEncoder[motorA] = 0;
	wait1Msec(500);
	float maxSpeed = (float)nMotorEncoder[motorA]/500
	displayBigTextLine(0, "%f", maxSpeed);
	motor[motorA] = 0;
	motor[motorB] = 0;
//while(!getButtonPress(buttonUp)) {}
	return maxSpeed;
}

void getTau(int power, float maxSpeed) {
	wait1Msec(1000);
	float sampleTime = 50;
	float encodesPerSampleTime_tauSpeed = maxSpeed * 0.632 * sampleTime;
	nMotorEncoder[motorA] = 0;
	float lastCheckEncode[5] = {99999, 99999, 99999, 99999, 99999};
	int i = 1;
	clearTimer(T1);
	motor[motorA] = power;
	motor[motorB] = power;
	do {
		while (time1[T1] < i * sampleTime / 5) {}
		for (int k = 4; k > 0; k--) {
			lastCheckEncode[k] = lastCheckEncode[k-1];
		}
		lastCheckEncode[0] = (float)nMotorEncoder[motorA];
		i += 1;
	} while ((float)nMotorEncoder[motorA] - lastCheckEncode[4] < encodesPerSampleTime_tauSpeed);
	float tau = sampleTime / 5 * (i-1);
	motor[motorA] = 0;
	motor[motorB] = 0;
	displayBigTextLine(3, "%f", tau);
	while(!getButtonPress(buttonUp)) {}
}

task main() {
	int power = 15;
	getTau(power, getMaxSpeed(power));
}
