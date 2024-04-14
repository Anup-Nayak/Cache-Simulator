all: run

run:
	g++ -Wall -Wextra -pedantic -g main.cpp -o cacheSim.exe
	./cacheSim.exe 32 16 16 write-allocate write-back lru < testgen.txt
	

clean:
	rm -f *.exe
	rm -f *.o

