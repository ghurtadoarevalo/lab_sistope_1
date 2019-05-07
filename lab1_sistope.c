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
    visibility->status = 1;

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
        child->fd_right = malloc(sizeof(int)*2);        
        child->fd_left = malloc(sizeof(int)*2);
    
        //Se abre el Pipe de ida
        if(pipe(child->fd_right) == -1)
        {
            perror("Error al crear el Pipe de ida");
            exit(-1);
        }

        //Se abre el Pipe de vuelta
        if(pipe(child->fd_left) == -1)
        {
            perror("Error al crear el Pipe de vuelta");
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
            dup2(child->fd_right[0],0); //Dup de lectura
            close(child->fd_right[1]); //Se cierra el escribir para el hijo
            dup2(child->fd_left[1],1); //Dup de escritura
            //close(child->fd_left[0]); //Se cierra el leer para el hijo
            close(child->fd_left[0]),
            execl("child","a","b", NULL);
            
            /*child->pid = getpid();// -> se copia el valor del pid child_struct del hijo
            
            printf("Soy el hijo: %d y mi papi es: %d\n", getpid(), getppid());
            
            visibility_s * visibility = malloc(sizeof(visibility_s));
            read(child->fd_right[0], visibility, sizeof(visibility_s));
            float test = distance(visibility);
            printf("Mi papi me está pasando origin_distance: %f con status: %d y yo soy: %d\n", test, visibility->status, getpid()) ;
            while(!(visibility->u == 0.f && visibility->v == 0.f && visibility->r == 0.f && visibility->i == 0.f && visibility->w == 0.f))
            {
                if (visibility->status == 1)
                {
                    float test = distance(visibility);
                    visibility->status = 0;
                    printf("Mi papi me está pasando origin_distance: %f con status: %d y yo soy: %d, en el while\n", test, visibility->status, getpid()) ;
                }
                read(child->fd_right[0], visibility, sizeof(visibility_s));

            }
            printf("Soy el hijo: %d y mi papi es: %d, me mataron\n", getpid(), getppid());

            write(child->fd_left[0], visibility, sizeof(visibility_s));*/

            exit(1);
        }

        else
        {
            child->pid = pid;// Se almacena el pid de todos los hijos creados, que seran las zonas de radio
            childsData->childs[i] = child;    
            close(child->fd_right[0]); //Se cierra el leer para el padre
            close(child->fd_right[1]); //Se cierra el escribir para el padre
            close(child->fd_left[0]); //Se cierra el leer para el padre
            close(child->fd_left[1]); //Se cierra el escribir para el padre
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
        radioList[i] = i*width;
   


    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        visibility_s * visibility = malloc(sizeof(visibility_s)); 
        buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
        visibility = buildVisibility(buf);

        /* Childs process */
        float origin_distance = distance(visibility);
        
        int i = 0;
        while(i < radio)
        {
            if(radioList[i] <= origin_distance && origin_distance < radioList[i+1]){
                printf("Entre en i: %d con origin_distance: %f\n",i,origin_distance);
                write(childsData->childs[i]->fd_right[1], visibility, sizeof(visibility_s));
            }

            i = i + 1;
        }
         /* End  childs */
    }

    for (int i = 0; i < radio+1; ++i)
    {
        visibility_s * visibility = malloc(sizeof(visibility_s)); 
        visibility->u = 0.f;
        visibility->v = 0.f;
        visibility->r = 0.f;
        visibility->i = 0.f;
        visibility->w = 0.f;
        write(childsData->childs[i]->fd_right[1], visibility, sizeof(visibility_s));
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

    //El padre espera a cada uno de los hijos
    for(int i = 0; i < radio+1; i++)
    {
        int status;
        waitpid(childsData->childs[i]->pid, &status,WUNTRACED | WCONTINUED);   

    }



    //El padre recibe los últimos mensajes de sus hijos
    char num[20];
    for(int i = 0; i < radio+1; i++)
    {
        float * results = malloc(sizeof(float)*4);
        read(childsData->childs[i]->fd_left[0], num, 20);
        printf("recibi el ultimo aliento de hijo: %d con results[0]: %s\n",childsData->childs[i]->pid, num);
    }

   

    //buildVisibilities();  

    return 0;
}
