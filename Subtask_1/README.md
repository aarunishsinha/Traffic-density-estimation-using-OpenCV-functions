# Traffic-density-estimation-using-OpenCV-functions
COP290 - Design Practices

## Subtask1: Camera angle correction and frame cropping
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
