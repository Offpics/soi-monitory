all: rm main

main:
	g++ main.cpp monitor.hpp monitor.h -lpthread -std=c++11 -o main

rm:
	rm main || true