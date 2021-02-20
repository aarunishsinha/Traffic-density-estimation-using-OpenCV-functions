#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

struct data{
    vector<Point2f> points;
    Mat img;
};

void mouseClick(int event, int x, int y, int flags, void* userdata){
    if (event==EVENT_LBUTTONDOWN){
        data *image = ((data *) userdata);
        imshow("Image", image->img);
        image->points.push_back(Point2f(x,y));
    }
}

int main( int argc, char** argv)
{

    // Read in the image.
    Mat im_src = imread("empty.jpg");

    // Converting RGB image to grayscale
    Mat grey_img;
    cvtColor(im_src, grey_img, COLOR_BGR2GRAY);

    MatSize s = grey_img.size;
    Size size(s[0], s[1]);
    // Mat im_projected = Mat::zeros(s, CV_8UC4);
    Mat im_projected (size, CV_8UC4);
    im_projected = 0;

    vector<Point2f> pts_projected;

    pts_projected.push_back(Point2f(0,0));
    pts_projected.push_back(Point2f(size.width-1,0));
    pts_projected.push_back(Point2f(size.width-1,size.height-1));
    pts_projected.push_back(Point2f(0,size.height-1));

    namedWindow("Image", 1);

    // Mouse Input
    Mat temp = grey_img.clone();

    data take_input;
    take_input.img = temp;

    setMouseCallback("Image", mouseClick, &take_input);

    imshow("Image", temp);
    waitKey(0);

    // Projecting image
    Mat transform = findHomography(take_input.points, pts_projected);
    warpPerspective(grey_img, im_projected, transform, size);

    imshow("Image", im_projected);
    waitKey(0);


    return 0;
}