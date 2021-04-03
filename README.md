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
```
$ make
```
To Run:
```
$ ./1_subtask <image_path_name>
```
The image given as input will appear in a window. Select 4 points, starting from top-left, in clockwise fashion and press ```Enter```.\
A new window will appear with the projected image and it will be stored in the same directory as well.\
Press ```Enter``` and another window will appear with the cropped image, which is also simultaneously stored in the same directory.\
Press ```Enter``` to terminate execution.

## Subtask 2: Queue and dynamic density estimation from traffic video
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
```
$ make
```
To Run:
```
$ ./2_subtask <video_path_name>
```
In order to run the program properly on a different video give an appropriate background image as input.
```
$ ./2_subtask <video_path_name> <bg_image_path_name>
```
The output will appear on the terminal window. First line of output is ```time(in secs),queue density,dynamic density``` followed by the frame number and the queue and dynamic density computed for that frame.
![alt text](https://github.com/aarunishsinha/Traffic-density-estimation-using-OpenCV-functions/blob/main/Subtask_2/plot.png "Traffic Densities vs Time")
