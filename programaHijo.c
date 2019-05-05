#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <math.h>
#include "functions.h"


float getMediaR(visibility_s *visibilityList, int len)
{
	float sumatoriaR = 0;

	for(int i = 0; i < len; i++)
		sumatoriaR = visibilityList[i].r + sumatoriaR;

	return sumatoriaR/len;
}	


float getMediaI(visibility_s *visibilityList, int len)
{
	float sumatoriaI = 0;

	for(int i = 0; i < len; i++)
		sumatoriaI = visibilityList[i].i + sumatoriaI;

	return sumatoriaI/len;
}


float getPotencia(visibility_s *visibilityList, int len)
{
	float sumatoriaW = 0;

	for(int i = 0; i < len; i++)
		sumatoriaW = sqrt(pow(visibilityList[i].r, 2) + pow(visibilityList[i].i, 2)) + sumatoriaW;

	return sumatoriaW;
}


float getRuido(visibility_s *visibilityList, int len)
{
	float sumatoriaR = 0;

	for(int i = 0; i < len; i++)
		sumatoriaR = visibilityList[i].w + sumatoriaR;

	return sumatoriaR;
}


int main(int argc, char const *argv[])
{
	







	return 0;
}








