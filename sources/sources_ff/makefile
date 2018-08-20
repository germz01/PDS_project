UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	CXX := g++-8
	CXXFLAGS := -D no_default_mapping -g -I./fastflow -I/usr/X11R6/include -O2 -std=c++17
else
	CXX := g++
	CXXFLAGS := -D no_default_mapping -g -I./fastflow -O2 -std=c++17
endif

LDFLAGS := -L/usr/X11R6/lib -pthread
LDLIBS := -lm -lstdc++fs -lX11

main: main_ff.cpp
	$(CXX) main_ff.cpp -o main $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)

.PHONY: clean

clean:
	rm main

