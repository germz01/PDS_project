test: test.cpp
	g++ test.cpp -o test -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
	./test

.PHONY: clean

clean:
	rm test
