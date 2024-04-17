all: run

run:
	g++ -Wall -Wextra -pedantic -g main.cpp -o cacheSim.exe
	./cacheSim.exe 256 4 16 write-allocate write-back fifo < traces/gcc.trace

clean:
	rm -f *.exe
	rm -f *.o

