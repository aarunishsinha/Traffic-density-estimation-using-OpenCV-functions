## Directory Structure
```
2018CS10321_2019CS10372_ass1_part3
	-> code
	-> analysis
		->method1
		->method2
		->method3
		->method4
		->method5
		->method6
		-> README.md
	-> Report.pdf
```
## method directory
Each method directory contains the outputs in ```.csv``` files. One csv file for each parameter and the value of the parameter is incorporated in the name of the ```.csv``` file.\
The output ```.csv``` files have the columns ```frame_number,queue_density,dynamic_density```, the runtime ```.csv``` has the columns ```parameter,runtime(in seconds)``` and the CPU utilisation ```.csv``` files have the columns ```parameter,CPU utilisation percentage```.\
There are script files(python files) that we have used for plotting the graphs.\
In the directory ```method4``` and ```method5``` there are two post-processing scripts (in python) that we have used to find the CPU utilisation on MacOS.