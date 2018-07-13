UNAME := $(shell uname)

main: main.cpp
ifeq ($(UNAME),Darwin)
	g++-8 main.cpp -o main -g -I/usr/X11R6/include -lstdc++fs -lX11 -L/usr/X11R6/lib -O3 -pthread -std=c++17
else
	g++ main.cpp -o main -g -lstdc++fs -lX11 -L/usr/X11R6/lib -O3 -pthread -std=c++17
endif

.PHONY: clean

clean:
	rm main

c_dir:
	rm -r ../output_dir*
