CC=g++

test: example.cpp
	$(CC) -std=c++11 $^ -I. -o plot

