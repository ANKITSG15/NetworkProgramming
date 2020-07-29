#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


//*****************************************************
 //Author -- Ankit Singh Gautam
 // Id-- 2019H1030518P

//*****************************************************
struct forkchild{
	pid_t pid;
	int status;
};

struct forkchild *cProcess;

int totalProcess;
FILE *fp;
 
void parentHandler(int snum, siginfo_t* data,void *ptr){

for(int k=0;k<totalProcess;k++)
{
	if(cProcess[k].pid==data->si_pid)
	{
		cProcess[k].status=1;
	}

}
}

void childHandler(int snum, siginfo_t* data,void *ptr){

	char *storage;
	size_t line_size=0;
	size_t line_buff=1024;
	

	line_size = getline(&storage,&line_buff,fp);
	printf("%s",storage);
	sleep(1);
	kill(getppid(),SIGUSR1);
}


int main(int argc, char * argv[])
{
	int flag=1;//Default for parent
    struct sigaction actionParent,actionChild;
    char buffer[1024];
    char * filename=argv[1];
    memset( buffer, '\0', sizeof(buffer));
	totalProcess = atoi(argv[2]);
    
    actionParent.sa_sigaction = parentHandler;
    sigemptyset(&actionParent.sa_mask);
    actionParent.sa_flags = SA_RESTART | SA_SIGINFO;

    cProcess =  malloc(totalProcess * sizeof(struct forkchild));	

	actionChild.sa_sigaction = childHandler;
    
    sigemptyset(&actionChild.sa_mask);
    actionChild.sa_flags = SA_RESTART | SA_SIGINFO;


    sigaction(SIGUSR1,&actionParent,NULL);
    
    fp = fopen(filename,"r");
    setvbuf(fp,buffer,_IONBF,1024);
	if(fp==NULL)
	{
		perror("Error while opening file");
	}	
 	


for(int j=0;j<totalProcess;j++)
{
	cProcess[j].pid = fork();
	cProcess[j].status =1; //child is free

	if(cProcess[j].pid==0)
	{//child
		sigaction(SIGUSR2,&actionChild,NULL);
		flag=0;
		break;
	}

}
if(flag==1){
while(1)
	{
		sleep(2);
		for(int i=0;i<totalProcess;i++)
	{

		if(cProcess[i].status==1){
			cProcess[i].status =0;
		//printf("Child Process going to-%d\n",cProcess[i].pid );
		kill(cProcess[i].pid,SIGUSR2);
		
	}
	}
	}
}
else if(flag==0){//child process
while(1){}
}
return 0;
}


