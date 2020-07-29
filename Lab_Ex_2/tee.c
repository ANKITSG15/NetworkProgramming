#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXSIZE 2048

int main(int argc, char * argv[])
{
char buff[MAXSIZE];
int files = argc-1;
FILE** fp= malloc(sizeof(FILE*)*(files));


for(int i=1;i<argc;i++)
{
  	fp[i]=fopen(argv[i],"w");
  	if(!(fp[i]))
  	{
  		perror("File:-");
  	}
}  			
while (fgets(buff, MAXSIZE, stdin) != NULL )
	{   
		printf("%s",buff);
  		for(int i=1;i<argc;i++){
  		fprintf(fp[i], "%s",buff);
 		fflush(fp[i]);
 	}
}

return 0;
}