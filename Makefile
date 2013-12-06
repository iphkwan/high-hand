all:
	g++ Track.cpp -o a.out `pkg-config --cflags --libs opencv`
clean:
	rm -rf a.out

