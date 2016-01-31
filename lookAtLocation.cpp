#include "lookAtLocation.h"


void lookAtLocation::lookAtLocationInit() {
	targetPort.open("/tutorial/target/in");
	Network::connect("/tutorial/target/out", "/tutorial/target/in");
	options.put("device", "remote_controlboard");
	options.put("local", "/tutorial/motor/client");
	options.put("remote", "/icubSim/head");
	robotHead = new PolyDriver(options);
	if (!robotHead->isValid()) {
		printf("Cannot connect to robot head\n");
	}
	
	robotHead->view(pos);
	robotHead->view(vel);
	vel->setVelocityMode();
	robotHead->view(enc);
	if (pos == NULL || vel == NULL || enc == NULL) {
		printf("Cannot get interface to robot head\n");
		robotHead->close();
	}
	jnts = 0;
	pos->getAxes(&jnts);
	setpoints.resize(jnts);
	printf("Joints %d", jnts);
}

lookAtLocation::lookAtLocation()
{
	lookAtLocationInit();
}

void lookAtLocation::doLook()
{
	Vector *target = targetPort.read();  // read a target
	if (target != NULL) { // check we actually got something
		//printf("We got a vector containing");
		for (size_t i = 0; i<target->size(); i++) 
		{
			//printf(" %g", (*target)[i]);
		}
		//printf("\n");

		double x = (*target)[0];
		double y = (*target)[1];
		double conf = (*target)[2];

		//x -= 320 / 2;
		//y -= 240 / 2;

		double vx = x*0.1;
		double vy = -y*0.1;

		// prepare command
		for (int i = 0; i<jnts; i++) 
		{
			setpoints[i] = 0;
		}
		if (conf>0.5) 
		{
			setpoints[3] = vy;
			setpoints[4] = vx;
		}
		else 
		{
			setpoints[3] = 0;
			setpoints[4] = 0;
		}
		vel->velocityMove(setpoints.data());
	}
}