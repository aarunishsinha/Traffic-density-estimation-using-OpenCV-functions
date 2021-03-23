#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <pthread.h>
// #include <X11/Xlib.h>                    // UNCOMMENT TO RESIZE WINDOW 

#define baseline_q 0.530452f
#define baseline_d 0.0511016f

using namespace cv;
using namespace std;

struct thread_data{
	int total_threads;
	int thread_id;
};

vector<float> global_queue;
vector<float> global_dynamic;
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

// DELETE THIS FUNCTION
void *wait(void *t) {
	int i;
    long tid;

    tid = (long)t;

    // sleep(1);
    // cout << "Sleeping in thread " << endl;
    cout << "Thread with id : " << tid << "  ...exiting " << endl;
    pthread_exit(NULL);
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
void *forkfunc(void *threadarg){
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;
	int num_threads = my_data->total_threads;
	int thread_num = my_data->thread_id;

	VideoCapture cap("../trafficvideo.mp4");
	if(!cap.isOpened()){
		cout<<"Error opening video file \n";
	}

	// Frame 1995(TS-> 2:13) set as background image.
	VideoCapture temp("../trafficvideo.mp4");
	temp.set(1,1995);
	Mat bgimg;
	temp>>bgimg;
	bgimg = cropFrame(bgimg);

	int cell_Height = bgimg.rows / num_threads;
    int cell_Width = bgimg.cols;

    // Splitting the Background image
    vector<Mat> bgimgSections(num_threads);
    int x = 0;
    int y = 0;
    int a = 0;

    for(y=0; y+cell_Height<=bgimg.rows; y+=cell_Height)
    {   
        bgimgSections[a] = bgimg(Rect(x, y, cell_Width, cell_Height));
        a++;
    }

    bgimg = bgimgSections[thread_num].clone();
    Mat prevFrame = bgimg.clone();

    float queue_density = 0.0;
    float dynamic_density = 0.0;
    float total_queue_density_in_split = 0.0;
    float total_dynamic_density_in_split = 0.0;
    int frame_count = 0;

    while(1){
    	Mat frame;
    	cap>>frame;
    	// video ends
		if(frame.empty()){
			break;
		}
    	Mat processedFrame = cropFrame(frame);
    	frame = processedFrame.clone();
    	frame_count++;

    	vector<Mat> frameSections(num_threads);
    	int x = 0;
    	int y = 0;
    	int a = 0;
    	for(y=0; y+cell_Height<=frame.rows; y+=cell_Height){
    		frameSections[a] = frame(Rect(x, y, cell_Width, cell_Height));
    		a++;
    	}
    	frame = frameSections[thread_num].clone();

    	Mat allVehicles = diffStatic(frame,bgimg);
    	queue_density = estimatedVehicle(allVehicles);
    	total_queue_density_in_split += queue_density;

    	Mat movingVehicles = diffMoving(frame,prevFrame);
    	dynamic_density = estimatedVehicle(movingVehicles);
    	total_dynamic_density_in_split += dynamic_density;

    	prevFrame = frame;
    }
    float avg_queue = total_queue_density_in_split / (float) frame_count;
    float avg_dynamic = total_dynamic_density_in_split / (float) frame_count;
    global_queue[thread_num] = avg_queue;
    global_dynamic[thread_num] = avg_dynamic;
    cap.release();
    pthread_exit(NULL);
}
void density_est(int& num_threads){
    // Initialising threads
    pthread_t threads[num_threads];
	time_t start, end;
	time(&start);
	void *status;

	struct thread_data td[num_threads];

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int rc;
	for(int i = 0; i<num_threads; i++){
		cout << "main() : creating thread, " << i << endl;
		td[i].thread_id = i;
		td[i].total_threads = num_threads;
		rc = pthread_create(&threads[i], &attr, forkfunc, (void *)&td[i]);
		if (rc) {
     		cout << "Error:unable to create thread," << rc << endl;
     		exit(-1);
  		}
	}
	pthread_attr_destroy(&attr);
		for(int i = 0; i < num_threads; i++ ) {
  		rc = pthread_join(threads[i], &status);
  		if (rc) {
     		cout << "Error:unable to join," << rc << endl;
     		exit(-1);
  		}
  		cout << "Main: completed thread id :" << i <<endl;
  	}
	
	time(&end);
	float q = 0;
	float d = 0;
	for(int i=0;i<num_threads;i++){
		q+= global_queue[i];
		d+= global_dynamic[i];
	}
	q = q / (float) num_threads;
	d = d / (float) num_threads;
	float squared_error_queue = (q - baseline_q)*(q - baseline_q);
	float squared_error_dynamic = (d - baseline_d)*(d - baseline_d);
	cout<<"Average queue_density ="<<q<<endl;
	cout<<"Average dynamic_density ="<<d<<endl;
	cout<<"Squared Error on Queue Density= "<<squared_error_queue<<endl;
	cout<<"Squared Error on Dynamic Density= "<<squared_error_dynamic<<endl;
	double time_taken = double(end - start); 
    cout << "Runtime = " << fixed 
         << time_taken << setprecision(5); 
    cout << " secs " << endl;
 //    method3<<x<<","<<y<<","<<squared_error_queue<<","<<squared_error_dynamic<<","<<time_taken<<"\n";
	// cap.release();
}


int main( int argc, char** argv)
{	
	int num_threads = 4;
	density_est(num_threads);
	// x = 3;
	// density_est(x,method1);
	return 0;
    
}
