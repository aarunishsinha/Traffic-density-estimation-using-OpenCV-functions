#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


int main( int argc, char** argv)
{

    // Read in the image.
    Mat im_src = imread("empty.jpg");

    // Converting RGB image to grayscale
    Mat grey_img;
    cvtColor(im_src, grey_img, COLOR_BGR2GRAY);

    namedWindow("Image", 1);

    imshow("Image", grey_img);
    waitKey(0);

    return 0;
}