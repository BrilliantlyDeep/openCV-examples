#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include <highgui.h>

using namespace cv;

int main (int argc, char** argv)
{
   Mat src, dst;

   src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
   dst.create(src.size(), src.type());

   // fill dst with the grayscaled version of src
   cvtColor(dst, dst, CV_RGB2GRAY);

   // lowthreshold = 100, highthreshold = 200, kernel size = 3
   Canny(dst, dst, 100, 200, 3);

   imwrite("edgedImage.jpeg", dst);

   return 0;
}
