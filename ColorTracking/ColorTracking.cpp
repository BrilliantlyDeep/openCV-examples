/*
   Simple code to track an object via webcam by its color.
   In the first version the code is looking for red objects.

   Date:    4th Marc 2017
   Author:  Mattia Iurich
   Version: 1.0
*/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define         LOW_HUE  0
#define        HIGH_HUE  179
#define  LOW_SATURATION  0
#define HIGH_SATURATION  255
#define       LOW_VALUE  255
#define      HIGH_VALUE  255

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
   VideoCapture cap(0);   // start capturing from webcam

   if(!cap.isOpened())
   {
      printf("Cannot access the webcam. Closing.\n");
      return -1;
   }

   // Create a window
   namedWindow("Control", CV_WINDOW_AUTOSIZE);

   int iLowH = 0;
   int iHighH = 179;
   int iLowS = 0;
   int iHighS = 255;
   int iLowV  = 0;
   int iHighV = 255;

   // Create trackbars to adjust the searched color
   // Hue 0-179
   cvCreateTrackbar("Low hue", "Control", &iLowH, 179);
   cvCreateTrackbar("High hue", "Control", &iHighH, 179);

   // Saturation 0-255
   cvCreateTrackbar("Low sat", "Control", &iLowS, 255);
   cvCreateTrackbar("High sat", "Control", &iHighS, 255);

   // Value 0-255
   cvCreateTrackbar("Low val", "Control", &iLowV, 255);
   cvCreateTrackbar("High val", "Control", &iHighV, 255);

   do
   {

      Mat src, srcHSV, srcThresholded;

      bool bSuccess = cap.read(src);
      if(!bSuccess)   // check if frames keep on coming
      {
         printf("Cannot read a frame from video stream\n");
         break;
      }

      /***** Here the algorithm starts *****/
      // Color conversion RGB to HSV
      cvtColor(src, srcHSV, COLOR_RGB2HSV);

      // Thresholding the image
      inRange(srcHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), srcThresholded);


      // Morphological operations
      // Morphological opening (remove small objects from background)
      erode(srcThresholded, srcThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
      dilate(srcThresholded, srcThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

      // Morphological closing (fill small holes in the foreground)
      dilate(srcThresholded, srcThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
      erode(srcThresholded, srcThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

      imshow("Thresholded image", srcThresholded);   // show the thresholded image
      imshow("Original", src);                       // compare it with the original

      if (waitKey(30) >= 0)   // exit if ESC key is pressed for atleast 30ms
      {
         printf("Exiting program.\n");
         break;
      }

   } while(true);

   return 0;
}
