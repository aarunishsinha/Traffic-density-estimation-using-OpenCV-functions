method1:
		g++ 3_sub_m_1.cpp -o 1_method -pthread -std=c++11 `pkg-config --cflags --libs opencv4`
method2:
		g++ 3_sub_m_2.cpp -o 2_method -pthread -std=c++11 `pkg-config --cflags --libs opencv4`
method3:
		g++ 3_sub_m_3.cpp -o 3_method -pthread -std=c++11 `pkg-config --cflags --libs opencv4`
clean1:
		rm ./1_method
		rm method1.txt
clean2:
		rm ./2_method

clean3:
		rm ./3_method
		rm method3.txt