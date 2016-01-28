// Tests.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv\highgui.h>

using namespace cv;

int main() {
	Mat image;          //Create Matrix to store image
	VideoCapture cap;          //initialize capture
	cap.open(0);
	namedWindow("window", 1);          //create window to show image
	while (1) {
		cap >> image;          //copy webcam stream to image
		imshow("window", image);          //print image to screen
		waitKey(33);          //delay 33ms
	}
	return 0;
}

