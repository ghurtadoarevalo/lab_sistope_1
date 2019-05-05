#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"
#include <math.h>

float distance(visibility_s * visibility)
{
    float distance = 0;
    distance = sqrt(pow(visibility->u,2) + pow(visibility->v,2));
    return distance; 
}


int main(int argc, char *argv[]) 
{
    printf("\n\n******************  CHILD PROGRAM %d  ******************\n\n", getpid());
    printf("Soy el hijo: %d y mi papi es: %d\n", getpid(), getppid());
    
    visibility_s * visibility = malloc(sizeof(visibility_s));
    read(0, visibility, sizeof(visibility_s));
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
        read(0, visibility, sizeof(visibility_s));

    }
    printf("Soy el hijo: %d y mi papi es: %d, me mataron\n", getpid(), getppid());

    write(1, visibility, sizeof(visibility_s));
    printf("\n\n******************  END CHILD %d  ******************\n\n", getpid());
}