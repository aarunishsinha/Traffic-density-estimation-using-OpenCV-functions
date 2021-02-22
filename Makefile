all: compile

compile:
		g++ 1_subtask.cpp -o 1_subtask -pthread -std=c++11 `pkg-config --cflags --libs opencv4`

# run1:
# 		./1_subtask empty.jpg

# run2:
# 		./1_subtask traffic.jpg

clean:
		rm 1_subtask

resize: compile2

compile2: 
		g++ 1_subtask.cpp -o 1_subtask -lX11 -pthread -std=c++11 `pkg-config --cflags --libs opencv4`