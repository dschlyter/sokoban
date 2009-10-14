#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstring>
#include <iostream>
#include "solver.h"

#define PORT 5555 
#define BUFFERSIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	char *ADDRESS ="cvap103.nada.kth.se";
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFERSIZE];
    if (argc < 2) {
       fprintf(stderr,"usage %s boardnum\n", argv[0]);
       exit(0);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(ADDRESS);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(PORT);
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    n = write(sockfd,argv[1],strlen(argv[1]));
    if (n < 0) 
         error("ERROR writing to socket");
    memset(buffer, 0, BUFFERSIZE);
    n = read(sockfd,buffer,BUFFERSIZE-1);
    if (n < 0) 
         error("ERROR reading from socket");
	printf("%s\n",buffer);
	Solver solver = Solver();
	char *MYSOL = solver.solve(buffer);	



	//char *MYSOL="U R R D U U L D L L U L L D R R R R L D D R U R U D L L U R";
    
	
	n = write(sockfd,MYSOL,strlen(MYSOL));
    if (n < 0) 
         error("ERROR writing to socket");
    memset(buffer, 0, BUFFERSIZE);
    n = read(sockfd,buffer,BUFFERSIZE-1);
	printf("%s\n",buffer);
    return 0;
}
