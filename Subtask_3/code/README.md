## To Compile:
```
$ make method<method_number>
```
For example, to compile method-1:
```
$ make method1
```

## To Execute:
Note: The path to the video is hard coded in each file as ```../trafficvideo.mp4``` so please keep the video in outside the ```code``` directory in the top directory.
### Method-1
```
$ ./1_method <x>
```
Where ```x``` is as specified in the assignment specifications.
### Method-2
```
$ ./2_method <X> <Y>
```
where ```X``` and ```Y``` define the new resolution ```XxY```.
### Method-3
```
$ ./3_method <NUM_THREADS>
```
### Method-4
```
$ ./4_method <NUM_THREADS>
```
### Method-5
```
$ ./5_method <NUM_THREADS>
```
### Method-5
```
$ ./6_method <NUM_THREADS>
```

## To Clean:
```
$ make clean<method_number>
```
This will remove the executable of that method from the directory.