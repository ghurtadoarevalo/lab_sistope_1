#include "functions.h"

int main(int argc, char const *argv[])
{
    char * fp_source_name_1 = "text.csv";
    int i, status;
    int radio = 3;
    int width = 60;
    float * results = malloc(sizeof(float)*4);
    childsData_s * childsData = createChilds(radio,width);
    
    readData(fp_source_name_1,radio,width, childsData);

    //El padre espera a cada uno de los hijos
    for(i = 0; i < radio+1; i++)
        waitpid(childsData->childs[i]->pid, &status,WUNTRACED | WCONTINUED);   

    //El padre recibe los últimos mensajes de sus hijos
    createOutFile("gg.txt");

    for(i = 0; i < radio+1; i++)
    {
        read(childsData->childs[i]->fd_left[0], results, sizeof(float)*4);
        printf("recibi el ultimo aliento de hijo: %d con results: %f %f %f %f\n",childsData->childs[i]->pid, results[0], results[1], results[2], results[3]);
        writeData(i+1, results, "gg.txt");
    }


    return 0;
}
