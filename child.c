#include "functions.h"

int main(int argc, char *argv[])
{
    visibility_s *visibility;
    visibilityList_s *visibilityList = NULL;
    float * results = calloc(5, sizeof(float));
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

    if (len > 0)
    {
    	results[0] = getAverageR(visibilityList, len);
	    results[1] = getAverageI(visibilityList, len);
	    results[2] = getPow(visibilityList);
	    results[3] = getNoise(visibilityList); 
        results[4] = len;
    }

    printf("Fin hijo\n");             
    write(1, results, sizeof(float)*5);


    return 0;
}
