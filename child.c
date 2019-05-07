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

void printDataList(visibilityList_s *visibilityList){
    visibilityList_s *auxVisibilityList = visibilityList;
    printf("Star data -------------\n");
    while(auxVisibilityList != NULL){
        printf("(%f %f %f %f %f)\n", (auxVisibilityList->visibility)->u, (auxVisibilityList->visibility)->v,
            (auxVisibilityList->visibility)->r, (auxVisibilityList->visibility)->i, (auxVisibilityList->visibility)->w);

        auxVisibilityList = auxVisibilityList->next; 
    } 
    printf("End data --------------\n");
}

int main(int argc, char const *argv[])
{
    visibility_s *visibility;
    visibilityList_s *visibilityList = NULL;
    int len = 0;

    do{
        visibility = malloc(sizeof(visibility_s));
        read(0, visibility, sizeof(visibility_s));

        if (visibility->status == 1)
        {
            visibility->status = 0;
            appendVisibility(&visibilityList, visibility);
            len++;
        }

    }while(!(visibility->u == 0.f && visibility->v == 0.f && visibility->r == 0.f && visibility->i == 0.f && visibility->w == 0.f));

    //printDataList(visibilityList);

    //printf("---([%d] %f, %f, %f, %f)---\n", len, getAverageR(visibilityList, len), getAverageI(visibilityList, len),
    //    getPow(visibilityList), getNoise(visibilityList));
    
    float * results = malloc(sizeof(float)*4);
    results[0] = getAverageR(visibilityList, len);
    results[1] = getAverageI(visibilityList, len);
    results[2] = getPow(visibilityList);
    results[3] = getNoise(visibilityList); 

    char num[20] = "hola";
    write(1, num, 100);

    return 0;
}
