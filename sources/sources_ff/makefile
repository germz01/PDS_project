UNAME := $(shell uname)

main: main_ff.cpp
ifeq ($(UNAME),Darwin)
	g++-8 main.cpp -o main -g -I./fastflow -I/usr/X11R6/include -lstdc++fs -lX11 -L/usr/X11R6/lib -O3 -pthread -std=c++17
else
	g++ main.cpp -o main -g -I./fastflow -lstdc++fs -lX11 -L/usr/X11R6/lib -O3 -pthread -std=c++17
endif

.PHONY: clean

clean:
	rm main

