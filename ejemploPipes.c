#include <stdio.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1

int main(int argc, char * argv[])
{
	int piped[2];
	int piped2[2];
	pipe(piped);
	pipe(piped2);
	char str[100];

	int pid = fork();
	if(pid == 0)
	{
		close(piped[WRITE]);
		read(piped[READ], str, 100);
		printf("Soy el hijo y recibo por el pipe: %s\n", str);
		close(piped2[READ]);
		write(piped2[WRITE], "No puedo, mañana tengo prueba de sistope!", 100);
	}
	else // Padre
	{
		close(piped[READ]);
		write(piped[WRITE], "Anda a acostarte, es muy tarde!", 100);
		close(piped2[WRITE]);
		read(piped2[READ], str, 100);
		printf("Soy el padre y recibo desde el pipe: %s\n", str);
		wait();
	}
}