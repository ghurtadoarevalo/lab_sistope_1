#include "functions.h"

int main(int argc, char *argv[])
{
    visibility_s *visibility;
    visibilityList_s *visibilityList = NULL;
    float * results = calloc(5, sizeof(float));
    int len = 0;

    //El hijo se mantiene escuchando hasta que llega una visibilidad con solo ceros, la cual es la manera de salir del ciclo.
    do{
        visibility = malloc(sizeof(visibility_s));
        read(0, visibility, sizeof(visibility_s));

        //Cuando el hijo lee una visibilidad nueva, es decir, que tenga status 1 y no 0, la almacena en una lista lista enlazada.
        if (visibility->status == 1)
        {
            visibility->status = 0;
            appendVisibility(&visibilityList, visibility);
            len++;
        }

    }while(!(visibility->u == 0.f && visibility->v == 0.f && visibility->r == 0.f && visibility->i == 0.f && visibility->w == 0.f));

    //Si llegó al menos 1 visibilidad, el hijo calcula lo solicitado, en caso contrario, devuelve un arreglo con sólo ceros.
    if (len > 0)
    {
    	results[0] = getAverageR(visibilityList, len);
	    results[1] = getAverageI(visibilityList, len);
	    results[2] = getPow(visibilityList);
	    results[3] = getNoise(visibilityList); 
        results[4] = len;
    }

    else
    {
        results[0] = 0;
        results[1] = 0;
        results[2] = 0;
        results[3] = 0;
        results[4] = 0;
    }

    write(1, results, sizeof(float)*5);

    //Si la bandera -b fue utilizada para ejecutar el programa, entonces el hijo imprime la última información.
    if(visibility->status == -1){
        fprintf(stderr, "Soy el hijo de pid %d, procese %d visibilidades\n", getpid(), len);
    }



    return 0;
}
