UNAME := $(shell uname)

main: main_ff.cpp
ifeq ($(UNAME),Darwin)
	g++-8 -std=c++17 -g -O3 -lstdc++fs -pthread -I/usr/X11R6/include -I./fastflow -L/usr/X11R6/lib -lX11 main_ff.cpp -o main
else
	g++ main.cpp -o main -g -O3 -std=c++17 -lstdc++fs -I./fastflow -L/usr/X11R6/lib -lX11 -pthread
endif

.PHONY: clean

clean:
	rm main

