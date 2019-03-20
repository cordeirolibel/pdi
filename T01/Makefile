all: clean_out output clean

output: cores.o desenho.o base.o imagem.o geometria.o main.o
	gcc cores.o desenho.o base.o imagem.o geometria.o main.o -o output -lm

cores.o: cores.c
	gcc -c cores.c

desenho.o: desenho.c
	gcc -c desenho.c

base.o: base.c
	gcc -c base.c

imagem.o: imagem.c
	gcc -c imagem.c

geometria.o: geometria.c
	gcc -c geometria.c

main.o: main.c
	gcc -c main.c

clean_out:
	rm -rf output
	rm -rf *.o

clean:
	rm -rf *.o