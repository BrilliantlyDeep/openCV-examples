/*
   Simple code to track an object via webcam by its color.
   What changed from v1.0 is that now it's trying to use
   the RGB space instead of the HSV one.

   Date: 5th March 2017
   Author: Mattia Iurich
   Version: 1.1
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
   VideoCapture cap(0);   // start capturing from webcam

   if(!cap.isOpened())   // Check if the webcam is accessible
   {
      printf("Cannot access the webcam. Closing.\n");
      return -1;
   }

   // Create a window
   namedWindow("Control", CV_WINDOW_AUTOSIZE);

   // Setting highs and lows for the color trackbars
   int LowRed    = 0;
   int HighRed   = 255;

   int LowGreen  = 0;
   int HighGreen = 255;

   int LowBlue   = 0;
   int HighBlue  = 255;

   // Trackbars
   // Red trackbar
   cvCreateTrackbar("Low red"   , "Control", &LowRed   , 255);
   cvCreateTrackbar("High red"  , "Control", &HighRed  , 255);

   // Green trackbar
   cvCreateTrackbar("Low green" , "Control", &LowGreen , 255);
   cvCreateTrackbar("High green", "Control", &HighGreen, 255);

   // Blue trackbar
   cvCreateTrackbar("Low blue",   "Control", &LowBlue  , 255);
   cvCreateTrackbar("High blue",  "Control", &HighBlue , 255);

   for(;;)
   {
      Mat frame, frameThresholded;
      cap >> frame; // Get a new frame from the webcam

      // Threshold the image
      inRange(frame, Scalar(LowRed, LowGreen, LowBlue), Scalar(HighRed, HighGreen, HighBlue), frameThresholded);

      // Morphological operations
      // Opening: remove small objects from the background
      erode (frameThresholded, frameThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
      dilate(frameThresholded, frameThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

      // Closing: fil small holes in the foreground
      dilate(frameThresholded, frameThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
      erode (frameThresholded, frameThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

      // Show images
      imshow("Original", frame);
      imshow("Thresholded", frameThresholded);

      if(waitKey(30) == 27)
      {
         printf("Exiting program\n");
         break;
      }

   }

   return 0;
}
