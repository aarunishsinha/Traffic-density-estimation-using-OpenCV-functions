#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"

#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <pthread.h>
// #include <X11/Xlib.h>                    // UNCOMMENT TO RESIZE WINDOW 

#define MaxFrames 500000

using namespace cv;
using namespace std;


static clock_t lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;

void init(){
    FILE* file;
    struct tms timeSample;
    char line[128];

    lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    file = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;
    while(fgets(line, 128, file) != NULL){
        if (strncmp(line, "processor", 9) == 0) numProcessors++;
    }
    fclose(file);
}

double getCurrentValue(){
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
        timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        percent = (timeSample.tms_stime - lastSysCPU) +
            (timeSample.tms_utime - lastUserCPU);
        percent /= (now - lastCPU);
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    return percent;
}



struct thread_data{
	int total_threads;
	int thread_id;
};

vector<float> global_queue(MaxFrames);
vector<float> global_dynamic(MaxFrames);
int TotalFrames;
pthread_mutex_t lock1;
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

void *forkfunc(void *threadarg){
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;
	int num_threads = my_data->total_threads;
	int thread_num = my_data->thread_id;
	
	// cout<<"start "<<thread_num<<"\n"; 
	
	VideoCapture cap("../trafficvideo.mp4");
	if(!cap.isOpened()){
		cout<<"Error opening video file \n";
	}
	int total_frame = cap.get(7);
	int start_frame = (total_frame/num_threads)*thread_num + min(total_frame%num_threads,thread_num);
	int end_frame = start_frame + (total_frame/num_threads) + (total_frame%num_threads>thread_num?1:0);
	// in case number of frames less than number of threads :(
	if(start_frame>=total_frame){
		cap.release();
		pthread_exit(NULL);		
	}
	// cout<<"start: "<<start_frame<<endl;
	// cout<<"end: "<<end_frame<<endl;
	// Frame 1995(TS-> 2:13) set as background image.
	VideoCapture temp("../trafficvideo.mp4");
	temp.set(1,1995);
	Mat bgimg;
	temp>>bgimg;
	temp.release();
	bgimg = cropFrame(bgimg);

    Mat prevFrame;
    if(thread_num==0){
    	cap.set(1,start_frame);
    	prevFrame = bgimg.clone();
    	TotalFrames = total_frame;
    }
    else{
    	cap.set(1,start_frame-1);
    	cap>>prevFrame;
    	prevFrame = cropFrame(prevFrame);
    }

    // cout <<thread_num<< " BG done"<<endl;

    float queue_density = 0.0;
    float dynamic_density = 0.0;
    int frame_count = start_frame;
    
    // cout <<thread_num<<" Entering Loop"<<endl;

    while(frame_count<end_frame){
    	Mat frame;
    	cap>>frame;
    	// video ends
		if(frame.empty()){
			break;
		}
    	Mat processedFrame = cropFrame(frame);
    	frame = processedFrame.clone();

    	// cout<<thread_num<< " Frame done: " << frame_count<<endl;

    	Mat allVehicles = diffStatic(frame,bgimg);
    	queue_density = estimatedVehicle(allVehicles);
		global_queue[frame_count] = queue_density;
		
    	Mat movingVehicles = diffMoving(frame,prevFrame);
    	dynamic_density = estimatedVehicle(movingVehicles);
		global_dynamic[frame_count] = dynamic_density;
		
    	// cout<<thread_num<< " Densities calculated: " << frame_count<<endl;

    	prevFrame = frame;
    	frame_count++;
    }

    // cout << thread_num<< " Stored in vector"<<endl;
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
	double time_taken = double(end - start); 
    cout << "Runtime = " << fixed 
         << time_taken << setprecision(5); 
    cout << " secs " << endl;
	
	ofstream fout;
	string filePath = to_string(num_threads)+"_out_method7.csv";
	fout.open(filePath);
	for(int i=0;i<TotalFrames;i++){ 
		fout<<i<<","<<global_queue[i]<<","<<global_dynamic[i]<<"\n";
	}
	fout.close();
	
	fout.open("runtime_method7.csv",ios::app);
	fout<<num_threads<<","<<time_taken<<"\n";
	fout.close();
}


int main( int argc, char** argv)
{	
	init();
	int num_threads = stoi(argv[1]);
	density_est(num_threads);
	double val = getCurrentValue();
	cout<<"cpu utilisation:"<<val<<endl;
	ofstream fout;
	fout.open("cpuUtilisation_method7.csv",ios::app);
	fout<<num_threads<<","<<val<<"\n";
	fout.close();
	
	return 0;
    
}







