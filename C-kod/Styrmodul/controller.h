

double p = 0.0;
double d = 0.0;
double K = 0.0;

int[] oldDistance;
int preferredDistance = 10;

double period = 0.2;
int index = 5;

int velocity = 100;

void StraightLineController(int frontDistance, int backDistance) 
{
	int distance = (frontDistance + backDistance) / 2;

	double p_out = P * (distance - preferredDistance);
	double d_out = D * (distance - oldDistance[index]) / (index * period);

	int y_out = K * (p_out + d_out);

	if(y < 0) {
		rightWheelPair(100 - y, 1);
		leftWheelPair(100, 1);

	} else {
		rightWheelPair(100, 1);
		leftWheelPair(100 - y, 1);

	}

	//TODO shift oldDistance;
	
}
