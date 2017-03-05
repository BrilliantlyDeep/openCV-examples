#include <iostream>
#include <cv.h>
#include <highgui.h>

using namespace cv;

int main(int argc, char** argv)
{
   Mat src, dst;

   src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
   dst.create(src.size(), src.type());

   cvtColor(src, dst, CV_RGB2GRAY);   // dst is now a grayscaled
                                      // copy of src

   Canny(dst, dst, 120, 170, 3);

   imwrite("edges.jpeg", dst);

   return 0;
}
