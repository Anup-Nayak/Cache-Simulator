all: run

run:
	g++ -g -Wall -Wextra -pedantic  main.cpp -o cacheSim
	# ./cacheSim 1 16384 16 write-allocate write-back lru < traces/swim.trace

clean:
	rm -f *.exe
	rm -f *.o

