#ifndef STRUCTS_H
#define STRUCTS_H

//Estructura que se utiliza para almacenar una visibilidad
typedef struct {
	float u;
	float v;
	float r;
	float i;
	float w;
} visibility_s;

//Estructura que mantiene una lista de los fd's de cada hijo
typedef struct {
	int * fd; // fd = [data,data];
	int pid;
} childData_s;

//Estructura que mantiene una lista de los fd's de cada hijo
typedef struct {
	childData_s ** childs; // childs = [[fd0,fd0,pid0],[fd1,fd1,pid1],...[fdN,fdN,pidN]] =>  N = cantidad de hijos creados
	int lenght;
}childsData_s;

#endif