#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#define PORT 12345
#define BUFSIZE 512

union semun
{
  int val;			
  struct semid_ds *buf;	
  unsigned short *array;
  struct seminfo *__buf;				
};

int main(int arg, char *argc[])
{
	int sock,bid,conAcc,cid;
	int cnt,sid;
	char buff[BUFSIZE];
	socklen_t addr_len;
	struct sockaddr_in servAddr;
	struct sockaddr_in clientAddr;
	struct sembuf sops;

	union semun semVar;
	semVar.val = atoi(argc[1]);//setting value for semaphore
	printf("Sema Value-%d\n",semVar.val);

	sid = semget(IPC_PRIVATE, 1,0644|IPC_CREAT);
	if(sid >=0){
	printf("semid--%d\n",sid);
	cnt = semctl(sid,0,SETVAL,semVar);
	if(cnt<0){
		perror("Error in cnt-\n");
		exit(1);
		}
	}

	memset(&servAddr,'\0',sizeof(servAddr));
	//sleep(10);
	sock = socket(AF_INET, SOCK_STREAM,0);
	if(sock<0)
	{
		perror("Error while creating socket\n");
		exit(1);
	}

	servAddr.sin_family =AF_INET;
	servAddr.sin_port = htons(PORT);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);


	bid = bind(sock,(struct sockaddr*)&servAddr,sizeof(servAddr));
	if(bid<0){
		printf("\nError in binding\n");
		exit(1);
	}

	if(listen(sock,20)==0){
		printf("\n--- Listening ---\n");
	}else{
		perror("Error in listening-");
		exit(1);
	}


	while(1){

	sops.sem_num=0;
	sops.sem_op=-1;
	sops.sem_flg=0;
	
		if(semop(sid,&sops,1)==-1){
			printf("Failed to acquire the lock\n");
			exit(1);
		}

		conAcc = accept(sock,(struct sockaddr*)&clientAddr,&addr_len);
		if(conAcc<0){perror("Error while connecting-");exit(1);}

		cid = fork();
		if(cid ==0){

			while(1){
				printf("client connecting\n");
				recv(conAcc,buff,BUFSIZE,0);
				if(strcmp(buff,"\n")==0){
					printf("Client sent the data, now time to disconnect once got newline");
					
					sops.sem_num=0;
					sops.sem_op=1;
					sops.sem_flg=0;
					if(semop(sid,&sops,1)==-1)
					{
						printf("Failed to release lock\n");
						exit(1);
					}
					
					printf("Lock Release\n");
					break;
				}


			}
		}
		
	}
	close(conAcc);

    return 0;

}
