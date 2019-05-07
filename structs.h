#ifndef STRUCTS_H
#define STRUCTS_H

//Estructura que se utiliza para almacenar una visibilidad
typedef struct {
	float u;
	float v;
	float r;
	float i;
	float w;
	int status;
} visibility_s;

//Estructura que mantiene una lista de los fd's de cada hijo
typedef struct {
	int * fd_right; // fd = [fd0,fd1];  pipe de ida 
	int * fd_left; // fd = [fd0,fd1];  pipe de vuelta 
	int pid;
} childData_s;

//Estructura que mantiene una lista de los fd's de cada hijo
typedef struct {
	childData_s ** childs; // childs = [[fd0,fd0,pid0],[fd1,fd1,pid1],...[fdN,fdN,pidN]] =>  N = cantidad de hijos creados
	int lenght;
}childsData_s;

struct visibilityList_s {
	visibility_s *visibility;
	struct visibilityList_s *next;
};
typedef struct visibilityList_s visibilityList_s;

#endif