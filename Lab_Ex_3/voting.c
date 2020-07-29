#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

int majority(int a[],int count);
int tossCoin();

struct my_msgbuf
{
  long mtype;
  int  mtext;
  int sentfrom;
};
struct forkchild{
	pid_t pid;
	int data;
	int returnVal;
};
struct forkchild *cProcess;

int totalProcess;
pid_t parentId;

// Start: Input as cmd line argv[1] : no. of child process to create
int main(int argc, char * argv[])
{
  int msqid;
  totalProcess = atoi(argv[1]); //Total number of child to create
  parentId = getpid();
  struct my_msgbuf buf;
  cProcess =  malloc(totalProcess * sizeof(struct forkchild));
 /*----Creating message queue and fetching queue id--------*/
  if(argc !=2 || totalProcess <=0){printf("Please put the command line argument as- <./voting> <# of Process(valid integer>0)\n");exit(1);}

 if ((msqid = msgget (IPC_PRIVATE, 0644)) == -1)
    { perror ("msgget"); }
 /*Forking n child based on command line argument*/
  for(int i=0;i<totalProcess;i++)
 {
   cProcess[i].returnVal = fork();
   if(cProcess[i].returnVal==0){
    cProcess[i].pid= getpid();
    break;
   }
 }

 if(parentId==getpid()){
	while(1){
			int* a = (int *)malloc(totalProcess * sizeof(int));
			/*-----Parent sending message to all childs-------*/
			printf("\n****************** PARENT sends a message to all childs ******************\n");
    		for(int i=0;i<totalProcess;i++)
    		{    
    			sleep(1);
    			buf.mtype =cProcess[i].returnVal;
    			buf.mtext = 1;
    			if((msgsnd (msqid, &buf, sizeof (buf), 0))==-1){
					perror("parent messaging-");
				}
			}
			sleep(1);
			
			/*---Parent receiving value from child and calculating majority----*/
			for(int i=0;i<totalProcess;i++){
				if((msgrcv (msqid, &(buf.mtype), sizeof (buf), getpid(), 0))==-1){
				perror("Receiving value from child-");}
				printf("Parent          : Receiving from Child Id- %d and the value chosen is %d\n",buf.sentfrom,buf.mtext);
		    	a[i] = buf.mtext;
		   		}
		    int result = majority(a,totalProcess);//accept:1 and reject:0
		    if(result==1){printf("Parent Decision : ACCEPT");}
		    else{printf("Parent Decision : REJECT");}		    
   }
}else{
 	while(1){
        sleep(2);
        /*-------Child receiving message from parent---------------------*/
       	if((msgrcv (msqid, &(buf.mtype), sizeof (buf), getpid(), 0))==-1){
    		perror("child receiving message from parent-");
    	}
    	/*------Child Sending----------*/
    	buf.mtext = tossCoin();
 		buf.mtype = parentId;
 		buf.sentfrom = getpid();
 		       printf("Child           : Child ID- %d chooses- %d\n",buf.sentfrom,buf.mtext);
 		if((msgsnd(msqid,&buf,sizeof(buf),0))==-1){
 			perror("Child Sending-");
 		}
 		

    }
   }

}

/*Functions to find the majority input1:array of values input2:size of array*/
int majority(int a[],int count){
int count0=0,count1=0;
for(int i=0;i<count;i++)
{
   if(a[i]==0)
   	{count0++;}
   else {count1++;}
}
if(count1>count0){return 1;}
else {return 0;}
}
/*Functions to find the random value 0 or 1*/
int tossCoin()
{

  srand(time(NULL));
  return(rand() % 2);
}

