uint8_t rotationSpeed = 0;
uint8_t noRotation = 127;

uint8_t rotationWheelSpeed;

int rotationSpeedInt = 0;
int oldRotationSpeedInt = 0;

double rotationSum = 0.0;
double oldRotationSum = 0.0;
double preferredRotation = 90.0;

double P_rotation = 0.0;
double D_rotation = 0.0;
double K_rotation = 0.0;

double sampleFrequency = 20.3;

void rotationController() {

	rotationSpeedInt = rotationSpeed - noRotation;

	rotationSum = rotationSum + (rotationSpeedInt + oldRotationSpeedInt) / 2 * 1 / sampleFrequency;

	double p_out_rotation = P_rotation * (rotationSum - preferredRotation);
	double d_out_rotation = D_rotation * rotationSpeedInt;

	rotationWheelSpeed = K_rotation * (p_out_rotation + d_out_rotation);

	if(rotationWheelSpeed < 0) {
		rightWheelPWM(rotationWheelSpeed, 1);
		leftWheelPWM(rotationWheelSpeed, 0);
	} else {
		rightWheelPWM(rotationWheelSpeed, 0);
		leftWheelPWM(rotationWheelSpeed, 1);
	}
}
