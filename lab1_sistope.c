#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "functions.h"

visibility_s * buildVisibility(char * readedData)
{
    visibility_s * visibility = malloc(sizeof(visibility_s));
    char * temp = malloc(sizeof(char)*(1024));
    strcpy(temp,readedData);
    char* token = NULL;
    token = strtok(temp, ",");
    visibility->u = atof(token);
    token = strtok(NULL, ",");
    visibility->v = atof(token);
    token = strtok(NULL, ",");
    visibility->r = atof(token);
    token = strtok(NULL, ",");
    visibility->i = atof(token);
    token = strtok(NULL, ",");
    visibility->w = atof(token);
    /*
    printf("U: %f ; ", visibilities->list[i-1]->u);
    printf("V: %f ;", visibilities->list[i-1]->v);
    printf("R: %f ;", visibilities->list[i-1]->r);
    printf("I: %f ;", visibilities->list[i-1]->i);
    printf("W: %f ;\n", visibilities->list[i-1]->w);
        */

    return visibility;
}

int readData(char * fp_source_name_1)
{
    FILE* fp;
    char buf[1024];
    /*if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <soure-file>\n", argv[0]);
        return 1;
    }*/

    if ((fp = fopen(fp_source_name_1, "r")) == NULL)
    { /* Open source file. */
        perror("fopen source-file");
        return 1;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        visibility_s * visibility = malloc(sizeof(visibility_s)); 
        buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
        visibility = buildVisibility(buf);
        printf("%f\n", visibility->w);
    }
    fclose(fp);
    return 0;

    
}


int main()
{
	char * fp_source_name_1 = "text.csv";
	
    readData(fp_source_name_1);
	//buildVisibilities();	

}