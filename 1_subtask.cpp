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

int main( int argc, char** argv)
{	
	
	// Initialize SCREEN_WIDTH and SCREEN_HEIGHT
    // getScreenResolution();                       // UNCOMMENT TO RESIZE WINDOW
	
	// Read the video file
	if (argc >3){
		cout<<"This program takes two arguements only \n";
		return -1;
	}
	else if(argc <2){
		cout<<"Please enter the path to video as an arguement \n";
		return -1;
	}
	VideoCapture cap(argv[1]);
	
	// check if capturing has been initialized properly
	if(!cap.isOpened()){
		cout<<"Error opening video file \n";
		return -1;
	}
	
	// Frame 1995(TS-> 2:13) set as background image.
	VideoCapture temp(argv[1]);
	temp.set(1,1995);
	Mat bgimg;
	if (argc >2){
		bgimg = imread(argv[2], IMREAD_COLOR);
		if (bgimg.empty()){
			cout<<"Error opening background image \n";
			return -1;
		}
	}
	else{
		temp>>bgimg;
	}
	bgimg = cropFrame(bgimg);
	Mat prevFrame = bgimg.clone();
	
	// For plotting
	ofstream frames;
	frames.open ("Plotting/frame_num.txt");
	ofstream q_dens;
	q_dens.open ("Plotting/queue_density.txt");
	ofstream d_dens;
	d_dens.open ("Plotting/dynamic_density.txt");
	ofstream output;
	output.open ("Plotting/out.txt");
	cout<<"framenum,queue density,dynamic density \n";

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
		//createWindow("Video Frame", processedFrame);
		//imshow("Video Frame", processedFrame);
		
		// remove background and highlight all the vehicles
		Mat allVehicles = diffStatic(processedFrame,bgimg);
		//createWindow("All Vehicles", allVehicles);
		//imshow("All Vehicles", allVehicles);
		
		// calculate vehicle count on road
		float queue_density= estimatedVehicle(allVehicles);
		q_dens<<queue_density<<",";
		frames<<frame_count<<",";
		
		// remove background using previous frame to highlight moving vehicles
		Mat movingVehicles = diffMoving(processedFrame,prevFrame);
		//createWindow("Moving Vehicles",movingVehicles);
		//imshow("Moving Vehicles",movingVehicles);
		
		// calculate moving vehicle count on road
		float dynamic_density = estimatedVehicle(movingVehicles);
		d_dens<<dynamic_density<<",";
		
		cout<<frame_count<<","<<queue_density<<","<<dynamic_density<<"\n";
		output<<frame_count<<","<<queue_density<<","<<dynamic_density<<"\n";
		
		prevFrame = processedFrame;
		
		// esc key pressed to exit from video
		// char ch=(char)waitKey(1);
		// if(ch==27){
		// 	break;
		// }
	}
	
	q_dens.close();
	frames.close();
	d_dens.close();
	output.close();
	cap.release();
	
	destroyAllWindows();
	
	return 0;
    
}
