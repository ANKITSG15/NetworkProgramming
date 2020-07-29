#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <unistd.h>
#define IP "127.0.0.1"
#define BUFFSIZE 2048


int main(int arg, char *argc[]){

int sock,bid,connfd,sockfd,port,bytesRead;
struct sockaddr_in servAddr;
struct sockaddr_in clientAddr;
fd_set readfds,evenfds,offfds,allfds;
int maxfd,maxi,Client[FD_SETSIZE];
char readBuff[BUFFSIZE],writeBuff[BUFFSIZE];
int readReady;
int clilen;
if(arg!=2){
	printf("Please run the file as ./<executable> <portnumer>\n");
	exit(1);
}

port = atoi(argc[1]);

sock = socket(AF_INET,SOCK_STREAM,0);
if(sock<0)
{
	perror("Error while creating socket-");
	exit(1);
}

memset(&servAddr,'\0',sizeof(servAddr));

servAddr.sin_family =AF_INET;
servAddr.sin_port = htons(port);
servAddr.sin_addr.s_addr = inet_addr(IP);

bid = bind(sock,(struct sockaddr*)&servAddr,sizeof(servAddr));
if(bid<0){
	printf("Error in port binding");
	exit(1);
}

if(listen(sock,20)!=0){
		perror("Error in listening-");
		exit(1);
}

maxfd = sock;
maxi = -1;

for(int i=0;i<FD_SETSIZE;i++){
 Client[i]=-1;
}

FD_ZERO(&allfds);
FD_SET(sock,&allfds);

for(;;){

	readfds = allfds;
	readReady = select (maxfd+1,&readfds,NULL,NULL,NULL);
	//printf("Select return-%d\n",readReady);
	if(FD_ISSET(sock,&readfds))
	{
		
		clilen = sizeof(clientAddr);
		connfd = accept(sock,(struct sockaddr *)&clientAddr,&clilen);
		
		printf("Client connected with connection fd- %d\n",connfd);
		
		for(int j=0;j<FD_SETSIZE;j++)
		{
			if(Client[j]<0)
			{
				Client[j]=connfd; 
				break;
			}
			/*if(j==FD_SETSIZE)
			{
				printf("Too Many clients-%d\n",j);
				 exit(1);
			}*/
			FD_SET(connfd,&allfds);

			if(connfd>maxfd) maxfd=connfd;
			if(j>maxi) maxi = j;
			if(--readReady<=0)
				continue;	
		}
	}

for(int k=0;k<=maxi;k++)
{
	
	if((sockfd=Client[k])<0){
		continue;
	}
	if(FD_ISSET(sockfd,&readfds)){
		if((bytesRead = read(sockfd,readBuff,BUFFSIZE))==0)
		{
			printf("No Data to Read-%d %d",sockfd,Client[k]);
			close(sockfd);
			FD_CLR(sockfd,&allfds);
			Client[k] =-1;
		}
		else{
			if(Client[k]%2==0){
				read(sockfd,readBuff,BUFFSIZE);
				printf("read buffer-checking even sending to odd%s\n",readBuff);
				for(int j=0;j<=maxi;j++)
				{
					if(Client[j]%2==1){
					write(Client[j],readBuff,BUFFSIZE);
				}	
			}
			if(Client[k]%2==1){
				read(sockfd,readBuff,BUFFSIZE);
				printf("read buffer-checking odd sending to even%s\n",readBuff);
				for(int j=0;j<=maxi;j++)
				{
					if(Client[j]%2==0){
					write(Client[j],readBuff,BUFFSIZE);
				}	
			}

		}
	}
}
if(--readReady <=0)break;
}
}

}
return 0;
}