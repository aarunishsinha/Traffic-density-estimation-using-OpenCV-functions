all: compile run

compile:
		g++ 1_subtask.cpp -o 1_subtask -lX11 -pthread -std=c++11 `pkg-config --cflags --libs opencv4`

run:
		./1_subtask

clean:
		rm 1_subtask
