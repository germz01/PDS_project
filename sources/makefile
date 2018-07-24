UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	CXX := g++-8
	CXXFLAGS := -g -I/usr/X11R6/include -O3 -std=c++17
else
	CXX := g++
	CXXFLAGS := -g -O3 -std=c++17
endif


LDFLAGS := -L/usr/X11R6/lib -pthread
LDLIBS := -lstdc++fs -lX11

main: main.cpp
	$(CXX) main.cpp -o main $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)

.PHONY: clean

clean:
	rm main

c_dir:
	rm -r ../output_dir*
