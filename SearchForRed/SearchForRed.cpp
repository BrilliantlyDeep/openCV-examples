#include <iostream>
#include <cv.h>
#include <highgui.h>

using namespace cv;

int main(int argc, char** argv)
{
   if (argc != 2)
   {
      printf("Error while loading the program!\n");
      return -1;
   }

   Mat src, dst;

   src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
   dst.create(src.size(), src.type());


   for (int x = 0; x < src.rows; x++)
      for (int y = 0; y < src.cols; y++)
      {
         if (src.at<Vec3b>(x,y)[2] >= 200 && src.at<Vec3b>(x,y)[0] <= 75 && src.at<Vec3b>(x,y)[1] <= 75)
         {
            dst.at<Vec3b>(x,y)[0] = 255;
            dst.at<Vec3b>(x,y)[1] = 255;
            dst.at<Vec3b>(x,y)[2] = 255;
         }
      }

   imwrite("redSpots.jpeg", dst);

   return 0;
}
