// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h> 
#include <string>
//#include "opencv2/imgproc/imgproc.hpp"
#include "lookAtLocation.h"

using namespace yarp::sig;
using namespace yarp::os;
//using namespace cv;

//Mat img;


void showVideo()
{
	Property p;
	p.put("device", "test_grabber");
	//p.put("subdevice", "test_grabber");
	p.put("name", "/test/video");
	p.put("mode", "ball");
	PolyDriver dev;
	dev.open(p);
	if (dev.isValid())
	{
		Network::connect("/test/video", "/icubSim/texture/screen");
		printf("CONNECTED VIDEO");
	}
	printf("CONNECTED VIDEO?");
}

int main(int argc, char *argv[])
{
	Network yarp;

	BufferedPort<ImageOf<PixelRgb> > imagePort;  // make a port for reading images
	BufferedPort<Vector> targetPort;
	showVideo();
	imagePort.open("/tutorial/image/in");  // give the port a name
	targetPort.open("/tutorial/target/out");
	Network::connect("/icubSim/cam", "/tutorial/image/in");
	lookAtLocation *look = new lookAtLocation();
	while (true) { // repeat forever
		ImageOf<PixelRgb> *image = imagePort.read();
		if (image != NULL) { // check we actually got something
							 //printf("We got an image of size %dx%d\n", image->width(), image->height());
			double xMean = 0;
			double yMean = 0;
			int ct = 0;
			for (int x = 0; x<image->width(); x++) {
				for (int y = 0; y<image->height(); y++) {
					PixelRgb& pixel = image->pixel(x, y);
					// very simple test for blueishness
					// make sure blue level exceeds red and green by a factor of 2
					if (pixel.b>pixel.r*1.2 + 10 && pixel.b>pixel.g*1.2 + 10) {
						// there's a blueish pixel at (x,y)!
						// let's find the average location of these pixels
						xMean += x;
						yMean += y;
						ct++;
					}
				}
			}
			if (ct>0) {
				xMean /= ct;
				yMean /= ct;
			}
			if (ct>(image->width() / 20)*(image->height() / 20)) {
				//printf("Best guess at blue target: %g %g\n", xMean, yMean);
				Vector &target = targetPort.prepare();
				target.resize(3);
				target[0] = xMean;
				target[1] = yMean;
				target[2] = 1;
				targetPort.write();
			}
			else {
				Vector& target = targetPort.prepare();
				target.resize(3);
				target[0] = 0;
				target[1] = 0;
				target[2] = 0;
				targetPort.write();
			}

			look->doLook();

		}
	}
	return 0;
}
