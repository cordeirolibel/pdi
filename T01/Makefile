all: clean_out output clean

texto: clean_out output_texto clean

output: cores.o desenho.o base.o imagem.o geometria.o main.o
	gcc cores.o desenho.o base.o imagem.o geometria.o main.o -o output -lm

output_texto: cores.o desenho.o base.o imagem.o geometria.o main_texto.o
	gcc cores.o desenho.o base.o imagem.o geometria.o main_texto.o -o output -lm

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

main_texto.o: main.c
	gcc -o main_texto.o -c -DTEXTO main.c

main.o: main.c
	gcc -c main.c

clean_out:
	rm -rf output
	rm -rf *.o

clean:
	rm -rf *.o