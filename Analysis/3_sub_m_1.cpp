#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
// #include <X11/Xlib.h>                    // UNCOMMENT TO RESIZE WINDOW 

#define baseline_q 0.530452f
#define baseline_d 0.0511016f

using namespace cv;
using namespace std;

int SCREEN_WIDTH,SCREEN_HEIGHT;

// Get Screen resolution of device being used
/*void getScreenResolution(){               // UNCOMMENT TO RESIZE WINDOW 
 	Display* disp = XOpenDisplay(NULL);
	Screen* screen = DefaultScreenOfDisplay(disp);
	SCREEN_WIDTH = screen->width;
	SCREEN_HEIGHT = screen->height;
}*/

 
void createWindow(string WindowName, Mat &img){
	// To resize image window if image resolution is more than screen resolution.
	
	namedWindow(WindowName, WINDOW_KEEPRATIO);
	
	/*if(img.size[1]>SCREEN_WIDTH || img.size[0]>SCREEN_HEIGHT){		// UNCOMMENT TO RESIZE WINDOW
		double scale_width= (double)(SCREEN_WIDTH)/img.size[1];
		double scale_height= (double)(SCREEN_HEIGHT)/img.size[0];
		double scale=min(scale_width,scale_height);
		
		int window_width=scale*img.size[1];
		int window_height=scale*img.size[0];
		
		resizeWindow(WindowName,window_width,window_height);
	}*/
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
	
    // Projecting image
    Mat transform = findHomography(pts_roadCorners, pts_projected);
    warpPerspective(grey_img, im_projected, transform, size);
    
    // Cropping image
    Rect croppedRectangle = Rect(750,210,350,840);
    Mat croppedImage = im_projected(croppedRectangle);
    
    return croppedImage;
}


Mat diffStatic(Mat& img, Mat& bg){
	Mat temp ;
	absdiff(img, bg, temp);
	for(int i=0;i<img.rows;i++){
		for(int j=0;j<img.cols;j++){
			int val = (int)(img.at<uchar>(i,j)) - (int)(bg.at<uchar>(i,j));
			if((val<=5 && val>=-40) || (val<=-85 && val>=-95)){
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
	dilate(er, er, getStructuringElement(MORPH_RECT,Size(7,7)));
	dilate(er, muler, getStructuringElement(MORPH_RECT,Size(7,7)));
	
	return muler;
}

Mat diffMoving(Mat& img, Mat& bg){
	Mat temp ;
	absdiff(img, bg, temp);
	for(int i=0;i<img.rows;i++){
		for(int j=0;j<img.cols;j++){
			int val = (int)(img.at<uchar>(i,j)) - (int)(bg.at<uchar>(i,j));
			if((val<=5 && val>=-2) || (val<=-5 && val>=-25)){
				temp.at<uchar>(i,j) = 0;
			}
			else{
				temp.at<uchar>(i,j) = 255;
			}
		}
	}
	
	Mat er;
	Mat muler;
	erode(temp, er, getStructuringElement(MORPH_RECT,Size(7,7)));
	dilate(er, er, getStructuringElement(MORPH_RECT,Size(7,7)));
	dilate(er, muler, getStructuringElement(MORPH_RECT,Size(7,7)));
	
	return muler;
}

Mat simpleDiff(Mat& img, Mat& bg){
	Mat temp ;
	absdiff(img, bg, temp);
	for(int i=0;i<img.rows;i++){
		for(int j=0;j<img.cols;j++){
			int val = (int)(img.at<uchar>(i,j)) - (int)(bg.at<uchar>(i,j));
			if(val<=5 && val>=-5){
				temp.at<uchar>(i,j) = 0;
			}
			else{
				temp.at<uchar>(i,j) = 255;
			}
		}
	}
	
	return temp;
}

float estimatedVehicle(Mat& res){
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
void density_est(int& x,ofstream& method1){
	VideoCapture cap("../trafficvideo.mp4");

	// check if capturing has been initialized properly
	if(!cap.isOpened()){
		cout<<"Error opening video file \n";
	}

	// Frame 1995(TS-> 2:13) set as background image.
	VideoCapture temp("../trafficvideo.mp4");
	temp.set(1,1995);
	Mat bgimg;
	temp>>bgimg;
	bgimg = cropFrame(bgimg);
	Mat prevFrame = bgimg.clone();

	int frame_count=0;
	float queue_density = 0.0;
	float dynamic_density = 0.0;
	// float total_queue_density = 0.0;
	// float total_dynamic_density = 0.0;
	time_t start, end;
	time(&start);
	while(1){
		Mat frame;
		cap>>frame;
		
		// video ends
		if(frame.empty()){
			break;
		}
		
		frame_count++;
		if(frame_count%x!=1 && x!=1){
			// total_queue_density+= queue_density;
			// total_dynamic_density+= dynamic_density;
			method1<<frame_count<<","<<queue_density<<","<<dynamic_density<<"\n";
			continue;
		}
		
		// project and crop the particular frame
		Mat processedFrame = cropFrame(frame);
		
		// remove background and highlight all the vehicles
		Mat allVehicles = diffStatic(processedFrame,bgimg);
		
		// calculate vehicle count on road
		queue_density= estimatedVehicle(allVehicles);
		// total_queue_density+= queue_density;
		
		// remove background using previous frame to highlight moving vehicles
		Mat movingVehicles = diffMoving(processedFrame,prevFrame);

		float time_secs = (float)frame_count / 15.0;
		// calculate moving vehicle count on road
		dynamic_density = estimatedVehicle(movingVehicles);
		// total_dynamic_density+= dynamic_density;
		
		method1<<frame_count<<","<<queue_density<<","<<dynamic_density<<"\n";
		
		prevFrame = processedFrame;
	}
	time(&end);

	// float avg_queue_density = total_queue_density / (float) frame_count;
	// float avg_dynamic_density = total_dynamic_density / (float) frame_count;
	// float squared_error_queue = (avg_queue_density - baseline_q)*(avg_queue_density - baseline_q);
	// float squared_error_dynamic = (avg_dynamic_density - baseline_d)*(avg_dynamic_density - baseline_d);
	// cout<<"Average queue_density ="<<avg_queue_density<<endl;
	// cout<<"Average dynamic_density ="<<avg_dynamic_density<<endl;
	// cout<<"Squared Error on Queue Density= "<<squared_error_queue<<endl;
	// cout<<"Squared Error on Dynamic Density= "<<squared_error_dynamic<<endl;

	double time_taken = double(end - start); 
    cout << "Runtime = " << fixed 
         << time_taken << setprecision(5); 
    cout << " secs " << endl;
    // method1<<x<<","<<squared_error_queue<<","<<squared_error_dynamic<<","<<time_taken<<"\n";
	cap.release();
	
	ofstream fout;
	fout.open("runtime_method1.csv",ios::app);
	fout<<x<<","<<time_taken<<"\n";
	fout.close();
}

int main( int argc, char** argv)
{	
	ofstream method1;
	char *p;
	int x = strtol(argv[1],&p,10);
	string filename = to_string(x)+"_out_method1.csv";
	method1.open (filename);
	density_est(x,method1);
	// x = 3;
	// density_est(x,method1);
	method1.close();
	return 0;
    
}
