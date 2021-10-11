exe: $(patsubst %.cpp, %.exe, $(wildcard *.cpp))

%.exe: %.cpp
	g++ -fopenmp -std=c++11 -o $@ $<