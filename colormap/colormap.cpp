#include <cv.h>
#include <highgui.h>
#include <stdio.h>

using namespace cv;

int main(int argc, char** argv)
{
   Mat src, dst[3];

   src = imread(argv[1], CV_LOAD_IMAGE_COLOR);

   split(src, dst);

   imwrite("blue.jpeg" , dst[0]);
   imwrite("green.jpeg", dst[1]);
   imwrite("red.jpeg",  dst[2]);

   return 0;
}
