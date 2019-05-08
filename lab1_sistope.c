#include "functions.h"

int main(int argc, char *argv[])
{
    int otp = 0, radio = 0, width = 0, flag = 0, i , status;
    char *nameFileIn = NULL, *nameFileOut = NULL;
    float *results = malloc(sizeof(float)*5);


    while((otp = getopt(argc, argv, ":i:o:n:d:b")) != -1)
    {
        if(otp == 'i')
            nameFileIn = optarg;

        else if(otp == 'o')
             nameFileOut = optarg;

        else if(otp == 'n')
            radio = atoi(optarg)-1;

        else if(otp == 'd')
            width = atoi(optarg);

        else if(otp == 'b')
            flag = 1;
        
        else
        {
            perror("Invalid Sintaxis");
            exit(-1);
        }
    }

    if(otp == -1 && argc == 1)
    {
       perror("Invalid Sintaxis");
        exit(-1); 
    }

    
    childsData_s * childsData = createChilds(radio);
    readData(nameFileIn, radio, width, childsData);
    //El padre espera a cada uno de los hijos
    for(i = 0; i < radio+1; i++)
        waitpid(childsData->childs[i]->pid, &status,WUNTRACED | WCONTINUED);   


    //El padre recibe los últimos mensajes de sus hijos
    createOutFile(nameFileOut);

    for(i = 0; i < radio+1; i++)
    {
        read(childsData->childs[i]->fd_left[0], results, sizeof(float)*5);
        if(flag == 1)
            printf("Soy el hijo de pid %d, procese %d visibilidades\n",childsData->childs[i]->pid, (int)results[4]);
        writeData(i+1, results, nameFileOut);
    }

    return 0;
}
