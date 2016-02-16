// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h> 
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "iCubLookManager.h"

using namespace yarp::sig;
using namespace yarp::os;
using namespace cv;

Mat img;
Mat bwImg;
int const max_BINARY_value = 255;

//connect video to the screen in the simuator
void showVideo()
{
	Property p;
	p.put("device", "test_grabber");
	p.put("name", "/test/video");
	p.put("mode", "ball");
	PolyDriver dev;
	dev.open(p);
	if (dev.isValid())
	{
		Network::connect("/test/video", "/icubSim/texture/screen");
	}
}

int main(int argc, char *argv[])
{
	//init yarp network
	Network yarp;

	//port for readin images
	BufferedPort<ImageOf<PixelBgr> > imagePort;
	//port for sending data to look at a location
	BufferedPort<yarp::sig::Vector> targetPort;

	showVideo();
	//open ports
	imagePort.open("/tutorial/image/in"); 
	targetPort.open("/tutorial/target/out");
	//connect camera to our image port
	Network::connect("/icubSim/cam/left", "/tutorial/image/in");

	//creat new lookatlocation which handles robot movement
	iCubLookManager *look = new iCubLookManager();

	while (true) {
		//read image from port
		ImageOf<PixelBgr> *image = imagePort.read();
		if (image != NULL) {

			//transform image to Mat for use in openCV
			IplImage* i = (IplImage*)image->getIplImage();
			img = cvarrToMat(i, true);
			//init Mat
			Mat bwImg(img.rows, img.cols, img.type());

			//make image black and white
			cvtColor(img, bwImg, CV_BGR2GRAY, CV_8UC3);

			//apply binary threshold
			threshold(bwImg, bwImg, 100, max_BINARY_value, THRESH_BINARY);

			//blur the image
			GaussianBlur(bwImg, bwImg, Size(9, 9), 2, 2);

			//hough circle detection
			vector <Vec3f> circles;
			HoughCircles(bwImg, circles, CV_HOUGH_GRADIENT, 1, 10, 100, 30, 0, 0);

			//comment the following 2 lines to make the program run without pressing a key
			imshow("Hough Circle Transform Demo", bwImg);
			waitKey(0);

			//go through all the circles found in the image and select the largest one (based on radius)
			int maxRadius = 0;
			int maxAcceptedRadius = 500;
			int maxRadiusCircleIndex = 0;
			for (int i = 0; i < circles.size(); i++)
			{
				int radius = cvRound(circles[i][2]);
				if (radius > maxRadius && radius <= maxAcceptedRadius)
				{
					maxRadius = radius;
					maxRadiusCircleIndex = i;
				}
			}

			if (maxRadius > 0) {
				//send circle coordinates to the targetPort which will be read by lookatlocation
				printf("Best guess at circle target: %g %g\n", circles[maxRadiusCircleIndex][0], circles[maxRadiusCircleIndex][1]);
				yarp::sig::Vector &target = targetPort.prepare();
				target.resize(3);
				target[0] = circles[maxRadiusCircleIndex][0];
				target[1] = circles[maxRadiusCircleIndex][1];
				target[2] = 1;
				targetPort.write();
			}else{
				//send coordinates 0, 0 and confidence 0 if didn't find a proper circle
				yarp::sig::Vector& target = targetPort.prepare();
				target.resize(3);
				target[0] = 0;
				target[1] = 0;
				target[2] = 0;
				targetPort.write();
			}

			//call doLook from lookat location, this checks the targetPort and moves the robot to look at that location
			look->doLook();

		}
	}
	return 0;
}
