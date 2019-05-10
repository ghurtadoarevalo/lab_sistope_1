#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "structs.h"

//Funcionalidades del padre
//-------------------------------------------------------------------------------------------------------------//

//Función que tomando una línea del archivo .csv genera una estructura llamada visiblidad
//la cual contiene U, V , R, I y W, además un status para saber si fue leido o no por algún hijo.
//Entrada: String que contiene U,V,R,I y W separados por comas.
//Salida: Estructura visiblidad.
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

    return visibility;
}

//Función que calcula una distancia (norma) utilizando U y V del vector.
//Entrada: Estructura visibilidad.
//Salida: La distancia calculada como un flotante.
float distance(visibility_s * visibility)
{
    float distance = 0;
    distance = sqrt(pow(visibility->u,2) + pow(visibility->v,2));
    return distance; 
}


//Funcion que genera tantos hijos como discos (radio + 1) existen, además genera 2 pipes por cada hijo,
//para luego establecer los dup2 y mantener los descriptores del pipe cuando se ejecute execl, lanzando
//un programa completamente nuevo para cada hijo, que realiza los cálculos de las visiblidades que envía el padre.
//Entrada: Cantidad de radios que habrán (discos - 1)
//Salida: Lista de información de hijos. Cada hijo contiene los descriptores de los pipes y el id del hijo.
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
            close(child->fd_left[0]), //Se cierra el leer para el hijo
            
            execl("Vis", "a", "b", NULL);

            printf("Error, no se pudo ejecutar execl, el programa se cerrará\n");
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

//Función que  lee línea por línea el archivo .csv y envía cada una de estas líneas al hijo que corresponda según su disco.
//Entrada: Nombre del archivo, cantidad de radios (discos -1), ancho del disco, información de los hijos y un flag para indicar
//si se muestra o no la información final de los hijos.
//Salida: Ninguna.
void readData(char * fp_source_name_1, int radio, int width, childsData_s * childsData, int flag)
{
    FILE* fp;
    char buf[1024];
    int i;
    float origin_distance;


    if ((fp = fopen(fp_source_name_1, "r")) == NULL)
    { 
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

        origin_distance = distance(visibility);
        i = 0;

        while(i < radio)
        {

            if(radioList[i] <= origin_distance && origin_distance < radioList[i+1])
            {
                write(childsData->childs[i]->fd_right[1], visibility, sizeof(visibility_s));
            }

            else if(i == radio -1 && radioList[i] <= origin_distance) //Este if es necesario para tomar al último hijo.
            { 
                write(childsData->childs[i+1]->fd_right[1], visibility, sizeof(visibility_s));
                break;
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

        //Si flag es 0, entonces se le envía al hijo un -2 para indicar que no debe mostrar la ultima informacion
        //Si flag es 1, entonces se le envía al hijo un -1 para indicar que debe mostrar la informacion.
        visibility->status = -1;
        if (flag == 0)
        {
            visibility->status = -2; 
        }
        
        write(childsData->childs[i]->fd_right[1], visibility, sizeof(visibility_s));
    }


    fclose(fp);
}

//Funcionalidades de los hijos
//-------------------------------------------------------------------------------------------------------------//

//Función que calcula la media real de todas las visibilidades. (1/len * Sumatoria(visibility->r))
//Entrada: Lista enlazada con las visibilidades y la cantidad de elementos de la lista.
//Salida: Número flotante con la media real.
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

//Función que calcula la media imaginaria de todas las visibilidades. (1/len * Sumatoria(visibility->i))
//Entrada: Lista enlazada con las visibilidades y la cantidad de elementos de la lista.
//Salida: Número flotante con la media imaginaria.
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

//Función que calcula la potencia de todas las visibilidades. (Sumatoria( raiz((visibility->r )**2 + (visibility->i)**2)))
//Entrada: Lista enlazada con las visibilidades y la cantidad de elementos de la lista.
//Salida: Número flotante con la potencia.
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

//Función que calcula el ruido total de las visibilidades. (Sumatoria(visibility->w))
//Entrada: Lista enlazada con las visibilidades y la cantidad de elementos de la lista.
//Salida: Número flotante con el ruido total.
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

//Funcion que permite agregar una visibilidad a la lista enlazada de visiblidades.
//Entrada: Lista enlazada de visibilidades, nueva visibilidad.
//Salida: Ninguna. 
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

//Funcion que crea un archivo con el nombre del string entrante.
//Entrada: Nombre del archivo de salida.
//Salida: Ninguna.
void createOutFile(char *outFileName)
{
    FILE *file = fopen(outFileName, "w");
    fclose(file);
}

//Función que escribe los resultados de los discos en el archivo de salida.
//Entrada: numero de disco, lista con los resultados del disco, nombre del archivo de salida.
//Salida: Ninguna.
void writeData(int number, float *results, char *outFileName)
{
	FILE *file = fopen(outFileName, "a");
	
	fprintf(file, "Disco %d:\nMedia real: %f\nMedia imaginaria: %f\nPotencia: %f\nRuido total: %f\n",
		number, results[0], results[1], results[2], results[3]);

	fclose(file);
}

#endif