all: run

run:
	g++ -Wall -Wextra -pedantic -g main.cpp -o cacheSim
	# ./cacheSim.exe 1 16384 16 write-allocate write-back lru < traces/swim.trace

clean:
	rm -f *.exe
	rm -f *.o

