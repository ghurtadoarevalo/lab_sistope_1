all: padre hijo 

padre: lab1.c
	gcc -o lab1 lab1.c -lm
hijo: Vis.c
	gcc -o Vis Vis.c -lm