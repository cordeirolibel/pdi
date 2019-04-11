all: clean_out output clean

output: main.o
	gcc main.o -o output -lm

main.o: main.c
	gcc -c main.c

clean_out:
	rm -rf output
	rm -rf *.o

clean:
	rm -rf *.o