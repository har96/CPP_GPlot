CC=g++

test: cpp_plot.cpp
	$(CC) -std=c++11 $^ -I. -o plot

