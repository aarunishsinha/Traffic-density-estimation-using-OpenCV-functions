# Traffic-density-estimation-using-OpenCV-functions
COP290 - Design Practices

## Subtask1: Camera angle correction and frame cropping
### Goals:
- Convert the images to grayscale.
- Add mouseclick functionality for selecting corresponding points.
- Perform projecting and cropping using OpenCV functions.
- Add user input for images and error-handling.

## Subtask2: Queue and dynamic density estimation from traffic video
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
$ ./1_subtask <video_path_name>
```
In order to run the program properly on a different video give an appropriate background image as input.
```
$ ./1_subtask <video_path_name> <bg_image_path_name>
```
The output will appear on the terminal window. First line of output is ```framenum,queue density,dynamic density``` followed by the frame number and the queue and dynamic density computed for that frame.
