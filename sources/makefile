UNAME := $(shell uname)

main: main.cpp
ifeq ($(UNAME),Darwin)
	g++-8 main.cpp -o main -g -std=c++17 -lstdc++fs -pthread -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
else
	g++ main.cpp -o main -g -std=c++17 -lstdc++fs -L/usr/X11R6/lib -lX11 -pthread
endif

.PHONY: clean

clean:
	rm main

c_dir:
	rm -r ../output_dir*
