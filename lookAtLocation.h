#pragma once
#include <stdio.h> 
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class lookAtLocation {
	public:
		lookAtLocation();
		void lookAtLocationInit();
		void doLook();
	private:
		Network yarp;
		BufferedPort<Vector> targetPort;
		Property options;
		IPositionControl *pos;
		IVelocityControl *vel;
		IEncoders *enc;
		int jnts;
		Vector setpoints;
		PolyDriver *robotHead;
};