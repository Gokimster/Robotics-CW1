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
#include "lookAtLocation.h"

using namespace yarp::sig;
using namespace yarp::os;
using namespace cv;

Mat img;
Mat bwImg;
int const max_BINARY_value = 255;


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

	BufferedPort<ImageOf<PixelBgr> > imagePort;  // make a port for reading images
	BufferedPort<yarp::sig::Vector> targetPort;
	showVideo();
	imagePort.open("/tutorial/image/in");  // give the port a name
	targetPort.open("/tutorial/target/out");
	Network::connect("/icubSim/cam/left", "/tutorial/image/in");
	//lookAtLocation *look = new lookAtLocation();
	while (true) {	 // repeat forever
		ImageOf<PixelBgr> *image = imagePort.read();
		if (image != NULL) { // check we actually got something
			printf("We got an image of size %dx%d\n", image->width(), image->height());

			double xMean = 0;
			double yMean = 0;
			int ct = 0;
			//printf("Made Mat \n");
			IplImage* i = (IplImage*)image->getIplImage();
			img = cvarrToMat(i, true);
			Mat bwImg(img.rows, img.cols, img.type());
			//printf("Values Mat \n");
			//make image black and white
			bool t = bwImg.type() == CV_8UC3;
			cv::cvtColor(img, bwImg, CV_BGR2GRAY, CV_8UC3);
			//printf("Greyscale \n");
			//apply binary threshold
			t = bwImg.type() == CV_8UC3;
			//imshow("Hough Circle Transform Demo", bwImg);
			threshold(bwImg, bwImg, 100, max_BINARY_value, 1);
			t = bwImg.type() == CV_8UC3;
			//imshow("Hough Circle Transform Demo", bwImg);
			//printf("Thresh \n");
			GaussianBlur(bwImg, bwImg, Size(9, 9), 2, 2);
			t = bwImg.type() == CV_8UC3;
			//printf("Blur \n");
			vector <Vec3f> circles;
			//Mat circles;
			//printf("Circles \n");
			bool s = bwImg.type() == CV_8UC3;
			//Canny(bwImg, bwImg, 50, 150, 3);
			HoughCircles(bwImg, circles, CV_HOUGH_GRADIENT, 1, 10,
				100, 30, 1, 30 // change the last two parameters
							   // (min_radius & max_radius) to detect larger circles
				);
			//namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
			//imshow("Hough Circle Transform Demo", bwImg);
			//waitKey(0);
			int maxRadius = 0;
			int maxAcceptedRadius = 500;
			int maxRadiusCircleIndex = 0;/*
			for (int i = 0; i < circles.size(); i++)//circles.size(); i++)
			{
				int radius = cvRound(circles[i][2]);
				if (radius > maxRadius && radius <= maxAcceptedRadius)
				{
					maxRadius = radius;
					maxRadiusCircleIndex = i;
				}
			}
			
			printf("done circles");
			if (maxRadius > 10) {
				printf("Best guess at circle target: %g %g\n", circles[maxRadiusCircleIndex][0], circles[maxRadiusCircleIndex][1]);
				yarp::sig::Vector &target = targetPort.prepare();
				target.resize(3);
				target[0] = circles[maxRadiusCircleIndex][0];
				target[1] = circles[maxRadiusCircleIndex][1];
				target[2] = 1;
				targetPort.write();
			}else*/ {
				yarp::sig::Vector& target = targetPort.prepare();
				target.resize(3);
				target[0] = 0;
				target[1] = 0;
				target[2] = 0;
				targetPort.write();
			}

			//look->doLook();

		}
	}
	return 0;
}
