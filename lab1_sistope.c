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
    int status;
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
            readedData * data = malloc(sizeof(readedData));
            read(child->fd[0], data, sizeof(data));
            printf("Mi papi me está pasando esto: %f con status: %d y yo soy: %d\n", data->number, data->status, getpid()) ;
            while(data->number != 0.f)
            {
                if (data->status == 1)
                {
                    data->status = 0;
                    printf("Soy el hijo: %d y mi papi es: %d, estoy en el while con data: %f\n", getpid(), getppid(), data->number);
                }
                read(child->fd[0], data, sizeof(data));

            }
            printf("Soy el hijo: %d y mi papi es: %d, me mataron\n", getpid(), getppid());

            exit(1);

            /*
            algo como proceso exec que termine con la ejecucion del hijo en el programa
            padre para que no se creen "nietos"
            */
            //exit(0); no puede ser exit, porque el hijo se cerraría, lo que necesitamos es que no haga nada más 
            break;
        }

        else
        {
            child->pid = pid;// Se almacena el pid de todos los hijos creados, que seran las zonas de radio
            childsData->childs[i] = child;    
            close(child->fd[0]); //Se cierra el leer para el padre
        }
    }

    return childsData;
}


int readData(char * fp_source_name_1, int radio, int width, childsData_s * childsData)
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

    int * radioList = malloc(sizeof(int)*radio+1); // radioList = [0, R1, R2, R3,..., Rn] -> n = numero de radios

    for (int i = 0; i < radio+1; ++i)
    {
        radioList[i] = i*width;
    }


    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        visibility_s * visibility = malloc(sizeof(visibility_s)); 
        buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
        visibility = buildVisibility(buf);

        /* Procesos Hijos */
        float origin_distance = distance(visibility);
        printf("%f\n",origin_distance);
        
        int i = 0;
        while(i < radio){
            if(radioList[i] <= origin_distance && origin_distance < radioList[i+1]){
                readedData * data = malloc(sizeof(readedData));
                float number;
                scanf("%f", &number);
                data->number = number;
                data->status = 1;
                write(childsData->childs[i]->fd[1], data, sizeof(data));
            }

            /*else if(i == n-1){
                execute child process i+1
                i = 100000;
            }*/

            i = i + 1;
        }

        /* Fin  */

    }
    fclose(fp);
    return 0;   
}


int main(int argc, char const *argv[])
{
	char * fp_source_name_1 = "text.csv";
    int radio = 3;
    int width = 60;

    childsData_s * childsData = createChilds(radio,width);
    readData(fp_source_name_1,radio,width, childsData);


    for(int i = 0; i < radio+1; i++)
    {
        readedData * data = malloc(sizeof(readedData));
        float number;
        scanf("%f", &number);
        data->number = number;
        data->status = 1;
        write(childsData->childs[0]->fd[1], data, sizeof(data));
        //printf("pid: %d\n", childsData->childs[i]->pid);
    }

    for(int i = 0; i < radio+1; i++)
    {
        readedData * data = malloc(sizeof(readedData));
        float number;
        scanf("%f", &number);
        data->number = number;
        data->status = 1;
        write(childsData->childs[1]->fd[1], data, sizeof(data));
        //printf("pid: %d\n", childsData->childs[i]->pid);
    }

    for(int i = 0; i < radio+1; i++)
    {
        int status;
        waitpid(childsData->childs[i]->pid, &status,WUNTRACED | WCONTINUED);   
    }




	//buildVisibilities();	

    return 0;
}
