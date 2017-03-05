#include <stdlib.h>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>

using namespace cv;

int main(int argc, char** argv)
{
   if (argc != 2)
   {
      printf("Error interpretating input argument formatting!\n");
      return -1;
   }

   Mat src, dst;

   // Load source image and copy its content into dst.
   src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
   dst.create(src.size(), src.type());

   // Convert dst to grayscale
   cvtColor(src, dst, CV_RGB2GRAY);

   // Apply Gaussian filter to the image.
   // Kernel size of the filter is set to be 3x3.
   // sigmaX and sigmaY components are set to be computed automatically
   // thanks to the getGaussianKernel() algorithm that sets them as:
   // simga = 0.3*((ksize - 1)*0.5 - 1) + 0.8.
   GaussianBlur(dst, dst, Size(3,3), 0, 0, BORDER_DEFAULT);

   // Thresholding operation: Otsu's binarization algorithm.
   threshold(dst, dst, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

   imwrite("thresholded_image.jpg", dst);

   return 0;
}
