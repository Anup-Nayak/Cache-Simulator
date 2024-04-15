all: run

run:
	g++ -Wall -Wextra -pedantic -g main.cpp -o cacheSim.exe
	./cacheSim.exe 256 64 16 write-allocate write-through fifo < testgen.txt
	

clean:
	rm -f *.exe
	rm -f *.o

