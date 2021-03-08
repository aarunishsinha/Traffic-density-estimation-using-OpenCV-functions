#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
// #include <X11/Xlib.h>                    // UNCOMMENT TO RESIZE WINDOW 

using namespace cv;
using namespace std;

int SCREEN_WIDTH,SCREEN_HEIGHT;

// Get Screen resolution of device being used
// void getScreenResolution(){               // UNCOMMENT TO RESIZE WINDOW 
// 	Display* disp = XOpenDisplay(NULL);
// 	Screen* screen = DefaultScreenOfDisplay(disp);
// 	SCREEN_WIDTH = screen->width;
// 	SCREEN_HEIGHT = screen->height;
// }

 
void createWindow(string WindowName, Mat &img){
	// To resize image window if image resolution is more than screen resolution.
	
	namedWindow(WindowName, WINDOW_KEEPRATIO);
	
	// if(img.size[1]>SCREEN_WIDTH || img.size[0]>SCREEN_HEIGHT){		// UNCOMMENT TO RESIZE WINDOW
	// 	double scale_width= (double)(SCREEN_WIDTH)/img.size[1];
	// 	double scale_height= (double)(SCREEN_HEIGHT)/img.size[0];
	// 	double scale=min(scale_width,scale_height);
		
	// 	int window_width=scale*img.size[1];
	// 	int window_height=scale*img.size[0];
		
	// 	resizeWindow(WindowName,window_width,window_height);
	// }
}

// Crop and warp a frame in a video
Mat cropFrame(Mat& im_src){
	// Converting RGB image to grayscale
    Mat grey_img;
    cvtColor(im_src, grey_img, COLOR_BGR2GRAY);

    MatSize s = grey_img.size;
    Size size(s[1], s[0]);
    // Mat im_projected = Mat::zeros(s, CV_8UC4);
    Mat im_projected (size, CV_8UC4);
    im_projected = 0;
	
	// // points to project the corner points of road
 //    vector<Point2f> pts_projected;
 //    pts_projected.push_back(Point2f(600,200));
 //    pts_projected.push_back(Point2f(1200,200));
 //    pts_projected.push_back(Point2f(1200,1000));
 //    pts_projected.push_back(Point2f(600,1000));
    
 //    // corner points of road in the empty.jpg image given in subtask1
 //    vector<Point2f> pts_roadCorners;
 //    pts_roadCorners.push_back(Point2f(950,213));
 //    pts_roadCorners.push_back(Point2f(1263,207));
 //    pts_roadCorners.push_back(Point2f(1515,1061));
 //    pts_roadCorners.push_back(Point2f(300,1073));

    vector<Point2f> pts_projected;
    pts_projected.push_back(Point2f(750,210));
    pts_projected.push_back(Point2f(1100,210));
    pts_projected.push_back(Point2f(1100,1050));
    pts_projected.push_back(Point2f(750,1050));
    
    // corner points of road in the empty.jpg image given in subtask1
    vector<Point2f> pts_roadCorners;
    pts_roadCorners.push_back(Point2f(981,213));
    pts_roadCorners.push_back(Point2f(1263,207));
    pts_roadCorners.push_back(Point2f(1515,1061));
    pts_roadCorners.push_back(Point2f(381,1073));
	
    // Projecting image
    Mat transform = findHomography(pts_roadCorners, pts_projected);
    warpPerspective(grey_img, im_projected, transform, size);
    
    // Cropping image
    Rect croppedRectangle = Rect(750,210,350,840);
    Mat croppedImage = im_projected(croppedRectangle);
    
    return croppedImage;
}


/*Mat diffImg(Mat& img, Mat& bg){
	Mat temp;
	absdiff(img, bg, temp);
	Mat dst;
	threshold( temp, dst, 60, 255, THRESH_BINARY );
	
	Mat res=dst;
	
	return res;
}*/


Mat diff(Mat& img, Mat& bg){
	//blur( img, img, Size(3,3) );
	Mat temp ;
	absdiff(img, bg, temp);
	for(int i=0;i<img.rows;i++){
		for(int j=0;j<img.cols;j++){
			int val = (int)(img.at<uchar>(i,j)) - (int)(bg.at<uchar>(i,j));
			if(val<=10 && val>=-95){
				temp.at<uchar>(i,j) = 0;
			}
			else{
				temp.at<uchar>(i,j) = 255;
			}
		}
	}
	
	Mat er;
	Mat muler;
	erode(temp, er, getStructuringElement(MORPH_RECT,Size(3,3)));
	dilate(er, muler, getStructuringElement(MORPH_RECT,Size(3,3)));
	
	return muler;
}


float estimatedVehicle(Mat& res){
	// int queue_density = 0;
	// vector<vector<Point>> contours;
	// vector<Vec4i> hierarchy;
	// findContours(res, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	// int total = 0;
	// for(auto cntr: contours){
	// 	if(contourArea(cntr)>=2500){
	// 		queue_density++;
	// 	}
	// 	total++;
	// }
	// float dens = (float)queue_density / (float)total;
	// return dens;
	int count = 0;
	int total = 0;
	for(int i=0;i<res.rows;i++){
		for(int j=0;j<res.cols;j++){
			if (res.at<uchar>(i,j) > 0){
				count++;
			}
			total++;
		}
	}
	float density = (float)count / (float)total;
	return density;
}


int main( int argc, char** argv)
{	
	
	// Initialize SCREEN_WIDTH and SCREEN_HEIGHT
    // getScreenResolution();                       // UNCOMMENT TO RESIZE WINDOW
    
	
	// Read the video file
	VideoCapture cap("trafficvideo.mp4");
	
	// check if capturing has been initialized properly
	if(!cap.isOpened()){
		cout<<"Error opening video file \n";
		return -1;
	}
	
	// Frame 1995(TS-> 2:15) set as background image.
	VideoCapture temp("trafficvideo.mp4");
	temp.set(1,1995);
	Mat bgimg;
	temp>>bgimg;
	bgimg = cropFrame(bgimg);
	Mat prevFrame = bgimg.clone();
	
	// For plotting
	ofstream frames;
	frames.open ("frame_num.txt");
	ofstream q_dens;
	q_dens.open ("queue_density.txt");
	ofstream d_dens;
	d_dens.open ("dynamic_density.txt");

	int frame_count=0;
	while(1){
		Mat frame;
		cap>>frame;
		
		// video ends
		if(frame.empty()){
			break;
		}
		
		frame_count++;
		if(frame_count%3!=1){
			continue;
		}
		
		// project and crop the particular frame
		Mat processedFrame = cropFrame(frame);
		createWindow("Video Frame", processedFrame);
		imshow("Video Frame", processedFrame);
		
		// remove background and highlight all the vehicles
		Mat allVehicles = diff(processedFrame,bgimg);
		createWindow("All Vehicles", allVehicles);
		imshow("All Vehicles", allVehicles);
		
		// calculate vehicle count on road
		float queue_density= estimatedVehicle(allVehicles);
		cout<<"Queue Density of frame "<<frame_count<<" is "<<queue_density<<"\n";
		q_dens<<queue_density<<",";
		frames<<frame_count<<",";
		
		// remove background using previous frame to highlight moving vehicles
		Mat movingVehicles = diff(processedFrame,prevFrame);
		createWindow("Moving Vehicles",movingVehicles);
		imshow("Moving Vehicles",movingVehicles);
		
		// calculate moving vehicle count on road
		float dynamic_density = estimatedVehicle(movingVehicles);
		cout<<"Dynamic Density of frame "<<frame_count<<" is "<<dynamic_density<<"\n";
		d_dens<<dynamic_density<<",";
		
		prevFrame = processedFrame;
		
		// esc key pressed to exit from video
		char ch=(char)waitKey(25);
		if(ch==27){
			break;
		}
	}
	
	q_dens.close();
	frames.close();
	d_dens.close();
	cap.release();
	
	destroyAllWindows();
	
	return 0;
    
}


/*
// points to project the corner points of road
    vector<Point2f> pts_projected;
    pts_projected.push_back(Point2f(750,210));
    pts_projected.push_back(Point2f(1100,210));
    pts_projected.push_back(Point2f(1100,1050));
    pts_projected.push_back(Point2f(750,1050));
    
    // corner points of road in the empty.jpg image given in subtask1
    vector<Point2f> pts_roadCorners;
    pts_roadCorners.push_back(Point2f(981,213));
    pts_roadCorners.push_back(Point2f(1263,207));
    pts_roadCorners.push_back(Point2f(1515,1061));
    pts_roadCorners.push_back(Point2f(381,1073));
*/
