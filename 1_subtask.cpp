#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <X11/Xlib.h>

using namespace cv;
using namespace std;

struct data{
    vector<Point2f> points;
    Mat img;
};

int SCREEN_WIDTH,SCREEN_HEIGHT;

void mouseClick(int event, int x, int y, int flags, void* userdata){
    if (event==EVENT_LBUTTONDOWN){
    	cout<<x<<" "<<y<<endl;
        data *image = ((data *) userdata);
        imshow("Image", image->img);
        image->points.push_back(Point2f(x,y));
    }
}

void getScreenResolution(){
	Display* disp = XOpenDisplay(NULL);
	Screen* screen = DefaultScreenOfDisplay(disp);
	SCREEN_WIDTH = screen->width;
	SCREEN_HEIGHT = screen->height;
}

 
void createWindow(string WindowName, Mat &img){
	// To resize image window if image resolution is more than screen resolution.
	
	namedWindow(WindowName, WINDOW_KEEPRATIO);
	
	if(img.size[1]>SCREEN_WIDTH || img.size[0]>SCREEN_HEIGHT){
		double scale_width= (double)(SCREEN_WIDTH)/img.size[1];
		double scale_height= (double)(SCREEN_HEIGHT)/img.size[0];
		double scale=min(scale_width,scale_height);
		
		int window_width=scale*img.size[1];
		int window_height=scale*img.size[0];
		
		resizeWindow(WindowName,window_width,window_height);
	}
}

int main( int argc, char** argv)
{	
	// Initialize SCREEN_WIDTH and SCREEN_HEIGHT
    getScreenResolution();
    

    // Read in the image.
    Mat im_src = imread("empty.jpg");
    

    // Converting RGB image to grayscale
    Mat grey_img;
    cvtColor(im_src, grey_img, COLOR_BGR2GRAY);

    MatSize s = grey_img.size;
    Size size(s[1], s[0]);
    // Mat im_projected = Mat::zeros(s, CV_8UC4);
    Mat im_projected (size, CV_8UC4);
    im_projected = 0;

    vector<Point2f> pts_projected;
    
    /*
    pts_projected.push_back(Point2f(0,0));
    pts_projected.push_back(Point2f(size.width-1,0));
    pts_projected.push_back(Point2f(size.width-1,size.height-1));
    pts_projected.push_back(Point2f(0,size.height-1));
    */
    
    pts_projected.push_back(Point2f(772,232));
    pts_projected.push_back(Point2f(1100,232));
    pts_projected.push_back(Point2f(1100,1010));
    pts_projected.push_back(Point2f(772,1010));
    
	
    // Mouse Input
    Mat temp = grey_img.clone();
	//namedWindow("Image", 1);
	createWindow("Image",temp);
	
    data take_input;
    take_input.img = temp;

    setMouseCallback("Image", mouseClick, &take_input);

    imshow("Image", temp);
    waitKey(0);
	destroyAllWindows();
	
	
    // Projecting image
    Mat transform = findHomography(take_input.points, pts_projected);
    warpPerspective(grey_img, im_projected, transform, size);
    
    createWindow("Projected Image", im_projected);
    imshow("Projected Image", im_projected);
    imwrite("projectedImage.jpg",im_projected);
    waitKey(0);
    
    
    // Cropping image
    Rect croppedRectangle = Rect(772,232,328,778);
    Mat croppedImage = im_projected(croppedRectangle);
    
    createWindow("Cropped",croppedImage);
    imshow("Cropped",croppedImage);
    imwrite("croppedImage.jpg",croppedImage);
    waitKey(0);
	
	destroyAllWindows();

    return 0;
}
