#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <X11/Xlib.h>                    // UNCOMMENT TO RESIZE WINDOW 

using namespace cv;
using namespace std;

struct data{
    vector<Point2f> points;
    Mat img;
};

int SCREEN_WIDTH,SCREEN_HEIGHT;
int MOUSE_INPUT_CNT;

void mouseClick(int event, int x, int y, int flags, void* userdata){
    if (event==EVENT_LBUTTONDOWN && MOUSE_INPUT_CNT<4){
    	MOUSE_INPUT_CNT++;
        data *image = ((data *) userdata);
        imshow("Image", image->img);
        image->points.push_back(Point2f(x,y));
    }
}

void getScreenResolution(){               // UNCOMMENT TO RESIZE WINDOW 
	Display* disp = XOpenDisplay(NULL);
	Screen* screen = DefaultScreenOfDisplay(disp);
	SCREEN_WIDTH = screen->width;
	SCREEN_HEIGHT = screen->height;
}

 
void createWindow(string WindowName, Mat &img){
	// To resize image window if image resolution is more than screen resolution.
	
	namedWindow(WindowName, WINDOW_KEEPRATIO);
	
	if(img.size[1]>SCREEN_WIDTH || img.size[0]>SCREEN_HEIGHT){		// UNCOMMENT TO RESIZE WINDOW
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
    getScreenResolution();                       // UNCOMMENT TO RESIZE WINDOW
	
	
	VideoCapture cap("trafficvideo.mp4");
	
	if(!cap.isOpened()){
		cout<<"Error opening video file \n";
		return -1;
	}
	
	while(1){
		Mat frame;
		cap>>frame;
		
		if(frame.empty()){
			break;
		}
		
		
		createWindow("Video Frame", frame);
		
		imshow("Video Frame",frame);
		
		// esc key pressed
		char ch=(char)waitKey(25);
		if(ch==27){
			break;
		}
	}
	
	cap.release();
	
	destroyAllWindows();
	
	return 0;


	
    /*
    
    if (argc < 2){
    	cout<<"Input filename required in the command line arguments! \n";
    	return -1;
    }
    
    // Read in the image.
    Mat im_src = imread(argv[1]);
    if (im_src.empty()){
        cout<<"Could not read the image "<<argv[1]<<endl;
        return -1;
    }
    

    // Converting RGB image to grayscale
    Mat grey_img;
    cvtColor(im_src, grey_img, COLOR_BGR2GRAY);

    MatSize s = grey_img.size;
    Size size(s[1], s[0]);
    // Mat im_projected = Mat::zeros(s, CV_8UC4);
    Mat im_projected (size, CV_8UC4);
    im_projected = 0;

    vector<Point2f> pts_projected;
    
    
    pts_projected.push_back(Point2f(772,232));
    pts_projected.push_back(Point2f(1100,232));
    pts_projected.push_back(Point2f(1100,1010));
    pts_projected.push_back(Point2f(772,1010));
    
	
    // Mouse Input
    Mat temp = grey_img.clone();
	createWindow("Image",temp);
	
    data take_input;
    take_input.img = temp;
	MOUSE_INPUT_CNT = 0;
    setMouseCallback("Image", mouseClick, &take_input);

    imshow("Image", temp);
    waitKey(0);
	destroyAllWindows();
	
	
    // Projecting image
    Mat transform = findHomography(take_input.points, pts_projected);
    warpPerspective(grey_img, im_projected, transform, size);
    
    createWindow("Projected Image", im_projected);
    string proj_name = string("proj_") + string(argv[1]);
    imshow("Projected Image", im_projected);
    imwrite(proj_name,im_projected);
    waitKey(0);
    
    
    // Cropping image
    Rect croppedRectangle = Rect(772,232,328,778);
    Mat croppedImage = im_projected(croppedRectangle);
    
    createWindow("Cropped",croppedImage);
    imshow("Cropped",croppedImage);
    string cropname = string("cropped_") + string(argv[1]);
    imwrite(cropname,croppedImage);
    waitKey(0);
	
	destroyAllWindows();

    return 0;*/
    
}
