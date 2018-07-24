UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	CXX := g++-8
	CXXFLAGS := -D no_default_mapping -g -I./fastflow -I/usr/X11R6/include -O3 -std=c++17
else
	CXX := g++
	CXXFLAGS := -D no_default_mapping -g -I./fastflow -O3 -std=c++17
endif

LDFLAGS := -L/usr/X11R6/lib -pthread
LDLIBS := -lstdc++fs -lX11

main: main_ff.cpp
	$(CXX) main_ff.cpp -o main $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)
# ifeq ($(UNAME),Darwin)
# 	g++-8 main_ff.cpp -o main -D no_default_mapping -g -I./fastflow -I/usr/X11R6/include -lstdc++fs -lX11 -L/usr/X11R6/lib -O3 -pthread -std=c++17
# else
# 	g++ main_ff.cpp -o main -D no_default_mapping -g -I./fastflow -lstdc++fs -lX11 -L/usr/X11R6/lib -O3 -pthread -std=c++17
# endif

.PHONY: clean

clean:
	rm main

