CC = g++
LIBS = `pkg-config --cflags --libs opencv`
OBJS = VideoDevice.o HandSegment.o HandTracker.o TraceAnalyser.o ResultDisplay.o main.o

all: main

#VideoDevice.o: VideoDevice.cpp
#	$(CC) -c $^

#HandSegment.o: HandSegment.cpp
#	$(CC) -c $^

#main.o: main.cpp
#	$(CC) -c $^

main: $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -rf main *.o
