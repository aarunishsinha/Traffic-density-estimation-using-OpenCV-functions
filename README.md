# Traffic-density-estimation-using-OpenCV-functions
COP290 - Design Practices

## Subtask 1: Camera angle correction and frame cropping
### Goals:
- Convert the images to grayscale.
- Add mouseclick functionality for selecting corresponding points.
- Perform projecting and cropping using OpenCV functions.
- Add user input for images and error-handling.

### README:
To Compile:
```shell
$ make
```
To Run:
```shell
$ ./1_subtask <image_path_name>
```
To Clean:
```shell
$ make clean
```
The image given as input will appear in a window. Select 4 points, starting from top-left, in clockwise fashion and press ```Enter```.\
A new window will appear with the projected image and it will be stored in the same directory as well.\
Press ```Enter``` and another window will appear with the cropped image, which is also simultaneously stored in the same directory.\
Press ```Enter``` to terminate execution.

## Subtask 2: Queue and dynamic density estimation from traffic video
Google Drive link for the [TrafficVideo](https://drive.google.com/drive/folders/12jChFq39G9d_o-8nxqPoAV-DGRaTl9X4?usp=sharing) used for density estimation.
### Goals:
- Take a video input and convert each frame to grayscale.
- Compute the queue density in each frame using a control (say an image of empty road).
- Compute the dynamic density in each frame.
- Plot a graph whose X-Axis is ```Time``` and Y-Axis is ```Density```.

### Implementation:
- We computed the densities for every 3rd frame in the video(starting from the first frame).
- We have chose the background image from density calculation from the video.
- For computing the dynamic density we used the previous frame used for computation as control to obseerve the displacement of the vehiles.
- Plotting has been done using Matplotlib.
### README:
To Compile:
```shell
$ make
```
To Run:
```shell
$ ./2_subtask <video_path_name>
```
In order to run the program properly on a different video give an appropriate background image as input.
```shell
$ ./2_subtask <video_path_name> <bg_image_path_name>
```
To Clean:
```shell
$ make clean
```
The output will appear on the terminal window. First line of output is ```time(in secs),queue density,dynamic density``` followed by the frame number and the queue and dynamic density computed for that frame.\

![alt text](https://github.com/aarunishsinha/Traffic-density-estimation-using-OpenCV-functions/blob/main/Subtask_2/plot.png "Traffic Densities vs Time")

## Subtask 3: Understanding and analyzing trade-offs in software design
## Goals:
- Try and implement different methods for traffic density estimation.
- Analyse each method for different parameters 
- Understand the trade-off in each method

## Methods:
### Method-1
Sub-sampling frames -- processing every x frame i.e. process frame `N` and then frame `N+x`, and for all intermediate frames just use the value obtained for N - total processing time will reduce, but utility might decrease as intermediate frames values might differ from baseline. Parameter for this method is x, how many frames you drop.
### Method-2
Reduce resolution for each frame. Lower resolution frames might be processed faster, but having higher errors. Parameter can be resolution XxY.
### Method-3 
Split work spatially across threads (application level pthreads) by giving each thread part of a frame to process. Parameter can be number of splits i.e. number of threads, if each thread gets one split. You might need to take care of boundary pixels that different threads process for utility.
### Method-4
Split work temporally across threads (application level pthreads), by giving consecutive frames to different threads for processing. Parameter can be number of threads.
### Method-5
Different implementation of method-3.
### Method-6
Split the video into equal chunks of small clips and process each clip on a different thread.

## Directory Structure:
```bash
├── Subtask_3
│   ├── analysis
│   │   ├── method1
│   │   │   └── ...
│   │   ├── method2
│   │   │   └── ...
│   │   ├── method3
│   │   │   └── ...
│   │   ├── method4
│   │   │   └── ...
│   │   ├── method5
│   │   │   └── ...
│   │   ├── method6
│   │   │   └── ...
│   │   └── Report.pdf
│   └── code
│   │   ├── 3_sub_m_1.cpp
│   │   ├── 3_sub_m_2.cpp
│   │   ├── 3_sub_m_3.cpp
│   │   ├── 3_sub_m_4.cpp
│   │   ├── 3_sub_m_5.cpp
│   │   ├── 3_sub_m_6.cpp
│   │   └── Makefile
```
## To Compile:
```shell
$ make method<method_number>
```
For example, to compile method-1:
```shell
$ make method1
```

## To Execute:
Note: The path to the video is hard coded in each file as ```../trafficvideo.mp4``` so please keep the video in outside the ```code``` directory in the top directory.
### Method-1
```shell
$ ./1_method <x>
```
Where ```x``` is as specified in the assignment specifications.
### Method-2
```shell
$ ./2_method <X> <Y>
```
where ```X``` and ```Y``` define the new resolution ```XxY```.
### Method-3
```shell
$ ./3_method <NUM_THREADS>
```
### Method-4
```shell
$ ./4_method <NUM_THREADS>
```
### Method-5
```shell
$ ./5_method <NUM_THREADS>
```
### Method-5
```shell
$ ./6_method <NUM_THREADS>
```

## To Clean:
```shell
$ make clean<method_number>
```
This will remove the executable of that method from the directory.

## Methods Directory
Each method directory contains the outputs in ```.csv``` files. One csv file for each parameter and the value of the parameter is incorporated in the name of the ```.csv``` file.\
The output ```.csv``` files have the columns ```frame_number,queue_density,dynamic_density```, the runtime ```.csv``` has the columns ```parameter,runtime(in seconds)``` and the CPU utilisation ```.csv``` files have the columns ```parameter,CPU utilisation percentage```.\
There are script files(python files) that we have used for plotting the graphs.\
In the directory ```method4``` and ```method5``` there are two post-processing scripts (in python) that we have used to find the CPU utilisation on MacOS.

## Analysis:
See [Report.pdf](https://github.com/aarunishsinha/Traffic-density-estimation-using-OpenCV-functions/blob/main/Subtask_3/analysis/Report.pdf) for detailed analysis and plots.
