#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "structs.h"

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


childsData_s * createChilds(int radiosQuantity)
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
            close(child->fd_left[0]),
            
            execl("child", "a", "b", NULL);
            
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
            close(child->fd_right[0]); //Se cierra el leer para el padre de ida
            close(child->fd_left[1]); //Se cierra el escribir para el padre de vuelta
        }
    }

    return childsData;
}


int readData(char * fp_source_name_1, int radio, int width, childsData_s * childsData)
{
    FILE* fp;
    char buf[1024];
    int i;
    float origin_distance;
    /*if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <soure-file>\n", argv[0]);
        return 1;
    }*/

    if ((fp = fopen(fp_source_name_1, "r")) == NULL)
    { /* Open source file. */
        perror("fopen source-file");
        exit(-1);
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
        origin_distance = distance(visibility);
        i = 0;

        while(i < radio)
        {
            if(radioList[i] <= origin_distance && origin_distance < radioList[i+1])
                write(childsData->childs[i]->fd_right[1], visibility, sizeof(visibility_s));


            else if(i == radio-1) // Completamente necesario! 
            { 
                write(childsData->childs[i+1]->fd_right[1], visibility, sizeof(visibility_s));
                i = 100000; 
            }

            i++;
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
//printf("----------------------------------------------------------------------\n");

float getAverageR(visibilityList_s *visibilityList, int len)
{
    float summR = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

    while(auxVisibilityList != NULL){
        summR = (auxVisibilityList->visibility)->r + summR;
        auxVisibilityList = auxVisibilityList->next;
    }

    return summR/len;
}   


float getAverageI(visibilityList_s *visibilityList, int len)
{
    float summI = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

    while(auxVisibilityList != NULL){
        summI = (auxVisibilityList->visibility)->i + summI;
        auxVisibilityList = auxVisibilityList->next;   
    }

    return summI/len;
}


float getPow(visibilityList_s *visibilityList)
{
    float summW = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

    while(auxVisibilityList != NULL){
        summW = sqrt(pow((auxVisibilityList->visibility)->r, 2) + pow((auxVisibilityList->visibility)->i, 2)) + summW;
        auxVisibilityList = auxVisibilityList->next;
    }

    return summW;
}


float getNoise(visibilityList_s *visibilityList)
{
    float summR = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

     while(auxVisibilityList != NULL){
        summR = (auxVisibilityList->visibility)->w + summR;
        auxVisibilityList = auxVisibilityList->next;
     }

    return summR;
}


void appendVisibility(visibilityList_s **visibilityList, visibility_s *newVisibility)
{
    if((*visibilityList) == NULL){
        (*visibilityList) =  malloc(sizeof(visibilityList_s));
        (*visibilityList)->visibility = newVisibility;
        (*visibilityList)->next = NULL;
    }
    else{
        visibilityList_s *newVisibilityList =  malloc(sizeof(visibilityList_s));   
        newVisibilityList->visibility = newVisibility;
        
        newVisibilityList->next = (*visibilityList);
        *visibilityList = newVisibilityList;  
    }
}

void printDataList(visibilityList_s *visibilityList)
{
    visibilityList_s *auxVisibilityList = visibilityList;
    //printf("Star data -------------\n");
    while(auxVisibilityList != NULL){
        //printf("(%f %f %f %f %f)\n", (auxVisibilityList->visibility)->u, (auxVisibilityList->visibility)->v,
         //   (auxVisibilityList->visibility)->r, (auxVisibilityList->visibility)->i, (auxVisibilityList->visibility)->w);

        auxVisibilityList = auxVisibilityList->next; 
    } 
    //printf("End data --------------\n");
}

void createOutFile(char *outFileName)
{
    FILE *file = fopen(outFileName, "w");
    fclose(file);
}

void writeData(int number, float *results, char *outFileName)
{
	FILE *file = fopen(outFileName, "a");
	
	fprintf(file, "Disco %d:\nMedia real: %f\nMedia imaginaria: %f\nPotencia: %f\nRuido total: %f\n",
		number, results[0], results[1], results[2], results[3]);

	fclose(file);
}

#endif