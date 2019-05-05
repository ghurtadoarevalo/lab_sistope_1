#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"
#include <math.h>

float distance(visibility_s *visibility)
{
    float distance = 0;
    distance = sqrt(pow(visibility->u,2) + pow(visibility->v,2));
    
    return distance; 
}

float getAverageR(visibilityList_s *visibilityList, int len)
{
    float summR = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

    while(auxVisibilityList->next != NULL)
        summR = (visibilityList->visibility)->r + summR;

    return summR/len;
}   


float getAverageI(visibilityList_s *visibilityList, int len)
{
    float summI = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

    while(auxVisibilityList->next != NULL)
        summI = (visibilityList->visibility)->i + summI;

    return summI/len;
}


float getPow(visibilityList_s *visibilityList)
{
    float summW = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

    while(auxVisibilityList->next != NULL)
        summW = sqrt(pow((visibilityList->visibility)->r, 2) + pow((visibilityList->visibility)->i, 2)) + summW;

    return summW;
}


float getNoise(visibilityList_s *visibilityList)
{
    float summR = 0;
    visibilityList_s *auxVisibilityList = visibilityList;

     while(auxVisibilityList->next != NULL)
        summR = (visibilityList->visibility)->w + summR;

    return summR;
}

visibilityList_s *createList(visibility_s *newVisibility)
{   
    visibilityList_s *visibilityList =  malloc(sizeof(visibilityList_s));
    visibilityList->visibility = malloc(sizeof(visibility_s));


    visibilityList->visibility = newVisibility;
    visibilityList->next = NULL;

    return visibilityList;
}

void appendVisibility(visibilityList_s **visibilityList, visibility_s *newVisibility)
{
    visibilityList_s *auxVisibility = *visibilityList;
    (auxVisibility->next)->visibility = newVisibility;
    (auxVisibility->next)->next = *visibilityList;
}


int main(int argc, char *argv[]) 
{
    printf("\n\n******************  CHILD PROGRAM %d  ******************\n\n", getpid());
    //printf("Soy el hijo: %d y mi papi es: %d\n", getpid(), getppid());
    
    visibility_s * visibility = malloc(sizeof(visibility_s));
    visibilityList_s *visibilityList;
    int len = 0;
    
    read(0, visibility, sizeof(visibility_s));
    float test = distance(visibility);

    visibilityList = createList(visibility);
    
    //printf("Mi papi me está pasando origin_distance: %f con status: %d y yo soy: %d\n", test, visibility->status, getpid()) ;
    
    while(!(visibility->u == 0.f && visibility->v == 0.f && visibility->r == 0.f && visibility->i == 0.f && visibility->w == 0.f))
    {
        if (visibility->status == 1)
        {
            float test = distance(visibility);
            visibility->status = 0;

            appendVisibility(&visibilityList, visibility);
            len = len + 1;
            //printf("Mi papi me está pasando origin_distance: %f con status: %d y yo soy: %d, en el while\n", test, visibility->status, getpid()) ;
            printf("(((((%d)))))\n", len);
        }
        read(0, visibility, sizeof(visibility_s));

    }


    printf("@@@@@@@@([%d] %f, %f, %f, %f)@@@@@@@@\n", len, getAverageR(visibilityList, len), getAverageI(visibilityList, len),
        getPow(visibilityList), getNoise(visibilityList));

    printf("Soy el hijo: %d y mi papi es: %d, me mataron\n", getpid(), getppid());

    write(1, visibility, sizeof(visibility_s));
    printf("\n\n******************  END CHILD %d  ******************\n\n", getpid());

    return 0;
}