/*
   Simple program to test the effectivness of the Canny Edge
   detection algorithm. The algorithm is structured as follows:
   1. apply gaussian filter to smooth the image in order to remove noise
   2. find the intensity gradients of the image
   3. apply non-maximum suppresstion to get rid of spurious response to edge detection
   4. apply double threshold to determine potential edges
   5. track edge by hysteresis: finalize detection of edges by suppressing all the other edges
      that are weak and not connected to strong edges.

   KERNEL_SIZE = size of the gaussian filter

   Date: 27th Feb 2017
   Author: Mattia Iurich
   Version: 1.0
*/

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

#define EDGE_THRESHOLD 1
#define MAX_LOW_THRESHOLD 100
#define KERNEL_SIZE 3
#define RATIO 3

Mat src, src_gray;	   // source image and its version greyscaled
                           // (note that the algorithm only works when the image is black & white

Mat dst, detected_edges;   // Destination image and its version with detected edges

int lowThreshold;
char windowName[] = "Edge Map";

void CannyThreshold(int, void*)
{
   // Reduce noise with a 3x3 kernel
   // blur(source, destination, kernel size)
   blur(src_gray, detected_edges, Size(3,3));

   // Canny detector
   Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*RATIO, KERNEL_SIZE);

   dst = Scalar::all(0);

   src.copyTo(dst, detected_edges);
   imshow(windowName, dst);
}

int main(int argc, char** argv)
{
   // load the image
   src = imread(argv[1]);

   if(!src.data)
      return -1;

   // create a matrix of the same type and size as src (for dst)
   dst.create(src.size(), src.type());

   // convert src to grayscale (use src_gray as resulting image)
   cvtColor(src, src_gray, CV_BGR2GRAY);

   // create a window
   namedWindow(windowName, CV_WINDOW_AUTOSIZE);

   // create a trackbar
   createTrackbar("Min threshold", windowName, &lowThreshold, MAX_LOW_THRESHOLD, CannyThreshold);

   // show the image
   CannyThreshold(0,0);


   // wait until user exits program by pressing a key
   waitKey(0);

   return 0;
}
