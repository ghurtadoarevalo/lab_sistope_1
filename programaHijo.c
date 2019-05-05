#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <math.h>
#include "functions.h"


float getAverageaR(visibility_s *visibilityList, int len)
{
	float summR = 0;

	for(int i = 0; i < len; i++)
		summR = visibilityList[i].r + summR;

	return summR/len;
}	


float getAverageI(visibility_s *visibilityList, int len)
{
	float summI = 0;

	for(int i = 0; i < len; i++)
		summI = visibilityList[i].i + summI;

	return summI/len;
}


float getPow(visibility_s *visibilityList, int len)
{
	float summW = 0;

	for(int i = 0; i < len; i++)
		summW = sqrt(pow(visibilityList[i].r, 2) + pow(visibilityList[i].i, 2)) + summW;

	return summW;
}


float getNoise(visibility_s *visibilityList, int len)
{
	float summR = 0;

	for(int i = 0; i < len; i++)
		summR = visibilityList[i].w + summR;

	return summR;
}


int main(int argc, char const *argv[])
{
	







	return 0;
}








