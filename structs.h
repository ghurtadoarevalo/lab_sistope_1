#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <math.h>

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
	int flag;
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

//------------------------------------------------------------

visibility_s * buildVisibility(char * readedData);

float distance(visibility_s * visibility);

childsData_s * createChilds(int radiosQuantity);

int readData(char * fp_source_name_1, int radio, int width, childsData_s * childsData);

//------------------------------------------------------------

float distance(visibility_s *visibility);

float getAverageR(visibilityList_s *visibilityList, int len);

float getAverageI(visibilityList_s *visibilityList, int len);

float getPow(visibilityList_s *visibilityList);

float getNoise(visibilityList_s *visibilityList);

void appendVisibility(visibilityList_s **visibilityList, visibility_s *newVisibility);

void printDataList(visibilityList_s *visibilityList);

void createOutFile(char *outFileName);

void writeData(int number, float *results, char *outFileName);

#endif