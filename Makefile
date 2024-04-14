all: run

run:
	g++ main.cpp -o cacheSim
	./cacheSim 4 4 16 write-allocate write-back fifo < testgen.txt


# -Wall -Wextra -pedantic -g