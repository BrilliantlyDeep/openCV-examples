/*
   The following code is meant to track an object by selecting its
   colour via a set of trackbars. It's inspired by Kyle Hounslow's code
   written in 2013.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software
   and associated documentation files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Date: 5th March 2017
   Author: Mattia Iurich
   Version: 1.0
*/

#include <sstream>
#include <string>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#define         LOW_HUE  0
#define        HIGH_HUE  179
#define  LOW_SATURATION  0
#define HIGH_SATURATION  255
#define       LOW_VALUE  0
#define      HIGH_VALUE  255

#define MIN_OBJECT_AREA  20*20
#define MAX_OBJECT_AREA  FRAME_HEIGHT*FRAME_WIDTH/1.5
#define MAX_NUM_OBJECTS  50

#define  FRAME_WIDTH  640
#define FRAME_HEIGHT  480

using namespace std;
using namespace cv;

void createTrackbars(const char* , int* , int* , int* , int* , int* , int*);
void morphOps(Mat &image);
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed);
void drawObject(int x, int y, Mat &frame);
string intToString(int number);

int main(int argc, char** argv)
{
   // Maximum and minimum values for Hue, Saturation, Value.
   int LowHue = 0;
   int HighHue = 179;
   int LowSat = 0;
   int HighSat = 255;
   int LowVal  = 0;
   int HighVal = 255;

   int x, y = 0;   // Coordinates

   Mat cameraFeed;    // Stores the frames of the incoming stream.
   Mat HSV;           // HSV converted image
   Mat Thresholded;   // Thresholded image

   // Create the trackbars for HSV filtering.
   createTrackbars("Trackbars", &LowHue, &HighHue, &LowSat, &HighSat, &LowVal, &HighVal);  

   // Capture frames from the webcam.
   // The size of the window is set manually (640x480).
   VideoCapture capture;
   capture.open(0);
   capture.set(CV_CAP_PROP_FRAME_WIDTH,  FRAME_WIDTH );
   capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

   for(;;)
   {
      capture.read(cameraFeed);   // store image to matrix
      cvtColor(cameraFeed, HSV, CV_BGR2HSV);   // Convert from RGB to HSV, store in HSV matrix
      // Perform the threshold filtering.
      inRange(HSV, Scalar(LowHue, LowSat, LowVal), Scalar(HighHue, HighSat, HighVal), Thresholded);
      morphOps(Thresholded);   // Perform the morphological operations.

      // Perform the object tracking
      trackFilteredObject(x, y, Thresholded, cameraFeed);

      imshow("Original", cameraFeed);
      imshow("Thresholded", Thresholded);
      
      if(waitKey(30) == 27)   // Exit if a button is pressed for at least 30ms
         break;
   }

   return 0;

}

void createTrackbars(const char* trackbarWindowName, int* pLowHue, int* pHighHue, int* pLowSat, int* pHighSat, int* pLowVal, int* pHighVal)
{
   // Create few trackbars in order to adjust the colour filtering

   namedWindow(trackbarWindowName, CV_WINDOW_AUTOSIZE);
   
   createTrackbar("Low Hue" , trackbarWindowName, pLowHue , HIGH_HUE       );
   createTrackbar("High Hue", trackbarWindowName, pHighHue, HIGH_HUE       );
   createTrackbar("Low Sat" , trackbarWindowName, pLowSat , HIGH_SATURATION);
   createTrackbar("High Sat", trackbarWindowName, pHighSat, HIGH_SATURATION);
   createTrackbar("Low Val" , trackbarWindowName, pLowVal , HIGH_VALUE     );
   createTrackbar("High Val", trackbarWindowName, pHighVal, HIGH_VALUE     );

   return;
}

void morphOps(Mat &image)
{
   // Morphological operations
   // Morphological opening (remove small objects from background)
   erode(image, image, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
   dilate(image, image, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

   // Morphological closing (fill small holes in the foreground)
   dilate(image, image, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
   erode(image, image, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

   return;
}

void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
{
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(x,y,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

void drawObject(int x, int y, Mat &frame)
{
    // Drawing crosshairs on tracked object

    circle(frame, Point(x,y), 20, Scalar(0,255,0),2);
    if (y-25>0)
       line (frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else
       line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    
    if (y+25<FRAME_HEIGHT)
       line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else
       line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    
    if (x-25>0)
       line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else
       line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    
    if (x+25 < FRAME_WIDTH)
       line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else
       line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

    stringstream sx, sy;
    sx << x;
    sy << y;
    
    putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

    return;
}

string intToString(int number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}

