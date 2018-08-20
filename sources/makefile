UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	CXX := g++-8
	CXXFLAGS := -g -I/usr/X11R6/include -O2 -std=c++17
else
	CXX := g++
	CXXFLAGS := -g -O2 -std=c++17
endif


LDFLAGS := -L/usr/X11R6/lib -pthread
LDLIBS := -lm -lstdc++fs -lX11

main: main.cpp
	$(CXX) main.cpp -o main $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)

.PHONY: clean

clean:
	rm main

c_dir:
	rm -r ../output_dir*
