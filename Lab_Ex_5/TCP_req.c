#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#define MAX 200
#include <time.h>

char *ipAddress;

char * getIPPort(char host[]){
	ipAddress = (char *)malloc(sizeof(char)*30);
	char *ip;
	char hostbuff[256];
	char **pptr;
	struct hostent *hptr;
	
	 if((hptr = gethostbyname(host))!= NULL)
	 {
	 	switch (hptr->h_addrtype) {
	 	case AF_INET:
	 					pptr= hptr -> h_addr_list;
	 					for(;*pptr!=NULL;pptr++)
	 					ip = inet_ntop(hptr->h_addrtype,*pptr,hostbuff,sizeof(hostbuff));
	   					strcpy(ipAddress,ip);
	 					//printf("\ngetIPPort()-%s",ipAddress); 
	 					break;
	 	default :
	 					 printf("Error"); break;

	 	}
	 }
return ipAddress;
}




int main(int argc, char *argv[])
{
 struct sockaddr_in servaddr;
 bzero(&servaddr,sizeof(servaddr));
 int sock;
 clock_t t;
 
 if(argc !=2){
 	printf("\nRun the program as ./<executable> <http://{hostname}.{uri}>\n");
 	exit(1);
 }
//Step-1. separating hostname and Domain name
 	char url[100];
 	char urlArr[100][100];
 	char *urlGlobal = (char *)malloc(100);
 	char *hostname = (char *) malloc(100);
 	char *uri = (char *)malloc(100);
	urlGlobal = argv[1];

	strcpy(url,urlGlobal); 
 	char *token = strtok(url,"/");
			
	int count =0;
	while(token!=NULL)
	{
		
		strcpy(urlArr[count],token);
		//printf("\nURL stored in urlArray-%s\n",urlArr[count]);
		//printf("%d-%s\n",count,token); 
        token = strtok(NULL, "/"); 
        count++;
	}

	strcpy(hostname,urlArr[1]);
	//printf("HostName(Copied)-%s",hostname);
	char *details = getIPPort(hostname);
	//printf("\nip inmain-%s\n",details);


	strcpy(uri,urlArr[count-1]);
	
	//Step-2. Creating socket and establishing connection with webserver.
  	sock = socket(AF_INET, SOCK_STREAM,0);
  	if(sock<0){perror("Error in creating socket-"); exit(1);}

  	servaddr.sin_family = AF_INET;
  	servaddr.sin_port = htons(80);
  	servaddr.sin_addr.s_addr = inet_addr(details);

  	if(connect(sock,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
  	{perror("Error while calling connect"); exit(1);}
  	
  	//Step-3. Making RequestLine, writing and reading to socket.



	char reqLine[MAX],respLine[MAX];
  	bzero(respLine,MAX);
  	snprintf(reqLine, MAX,"GET /%s HTTP/1.1\r\n""Host: %s\r\n""\r\n", uri, hostname);
  	printf("\nRequestBody-%s",reqLine);

  	int writeCount = write(sock,reqLine,strlen(reqLine));

  	if(writeCount<=0)
  	{printf("Nothing is written to socket, please check the RequestLine");exit(1);}
  	
  	//printf("SocketWrite-%d\n",writeCount);
  	t = clock();
  	if(writeCount)
  	{
	  	while(read(sock,respLine,MAX)!=0)
  		{	
  			respLine[MAX] = '\0';
  			if(fputs(respLine,stdout)==EOF)
  				{perror("fputs error");}
  			printf("%s",respLine);
  		}
  		
	 }
  	
  	t = clock() - t;

	double timeElapsed = ((double)t)/CLOCKS_PER_SEC;

	printf("\n******************Time to get the output-> %f seconds************************ ",timeElapsed);

close(sock);
return 0;
}
