#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <math.h>
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

float distance(visibility_s * visibility)
{
    float distance = 0;
    distance = sqrt(pow(visibility->u,2) + pow(visibility->v,2));
    return distance; 
}

void sendData(visibility_s * visibility)
{

}

childsData_s * createChilds(int radiosQuantity, int height)
{
    childsData_s * childsData = malloc(sizeof(childsData_s));
    childsData->childs = malloc(sizeof(childData_s)*radiosQuantity+1);
    childsData->lenght = radiosQuantity+1;
    int pid;
    
    for(int i = 0; i < radiosQuantity+1; i++)
    {
        childData_s * child = malloc(sizeof(child));
        child->fd = malloc(sizeof(int)*2);

        //Se abre el Pipe
        if(pipe(child->fd) == -1)
        {
            perror("Error al crear el Pipe");
            exit(-1);
        }

        //Se crea el hijo
        if((pid = fork()) < 0)
        {
            perror("Te lo echaste");
            exit(-1);
        }

        if(pid == 0)
        {
            child->pid = getpid();// -> se copia el valor del pid child_struct del hijo
            printf("Soy el hijo: %d y mi papi es: %d\n", getpid(), getppid());
            /*
            algo como proceso exec que termine con la ejecucion del hijo en el programa
            padre para que no se creen "nietos"
            */
            exit(0); 
        }
        child->pid = getpid();// -> siempre el mismo, el del padre

        childsData->childs[i] = child;
    }
    wait(NULL);

    return childsData;
}


int readData(char * fp_source_name_1, int radio, int width)
{
    FILE* fp;
    char buf[1024];
    int origin_distance;
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

        /* Procesos Hijos */
        origin_distance = distance(visibility);

        /* listaRadio = [0, R1, R2, R3,..., Rn] -> n = numero de radios
        int i = 0;
        while(i < n){
            if(listaRadio[i] <= origin_distance && origin_distance < listaRadio[i+1]){
                execute child process i -> ni idea como hacer eso
                i = 100000;
            }

            else if(i == n-1){
                execute child process i+1
                i = 100000;
            }

            i = i + 1;
        }
        */

        /* Fin  */

    }
    fclose(fp);
    return 0;   
}


int main(int argc, char const *argv[])
{
	char * fp_source_name_1 = "text.csv";
    int radio = 3;
    int width = 3;

    childsData_s * childsData = createChilds(radio,width);
    readData(fp_source_name_1,radio,width);

    for(int i = 0; i < radio+1; i++)
    {
        printf("pid: %d\n", childsData->childs[i]->pid);
    }


	//buildVisibilities();	

    return 0;
}
