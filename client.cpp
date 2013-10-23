#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstring>
#include <iostream>
#include "solver.h"
#include "workerthread.h"

#define PORT 5032
#define BUFFERSIZE 10024

pthread_mutex_t qMutex;
pthread_mutex_t sMutex;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	char *ADDRESS ="dd2380.csc.kth.se";
	//char *ADDRESS ="localhost";
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
    cout << "Recieved map from server: " << endl;
	printf("%s\n",buffer);

	int threads = 4;
	int chunksize = 300;
	if (argc > 2)
		threads = atoi(argv[2]);
	if (argc > 3) 
		chunksize = atoi(argv[3]);


	Solver * solver = new Solver(chunksize);
	solver->init(buffer);

	cout << "Running with " << threads << " threads," << endl;
	cout << "and chunksize: " << chunksize << endl << endl;


	ThreadPtr * threadArray = new ThreadPtr[threads];

	for (int i = 0; i < threads; i++) {
		threadArray[i] = ThreadPtr(new WorkerThread(solver, i));
		threadArray[i]->start();
		//cout << "Started thread: " << i << endl << flush;
	}
	
	for (int i = 0; i < threads; i++) {
		//cout << "Waiting for thread " << i << "..." << endl << flush;
		threadArray[i]->wait();
		//cout << "Thread " << i << " finished." << endl << flush;
	}

	char *MYSOL = solver->solution;
	delete[] threadArray;
	delete solver;
	//cout << "String is: \"" << solver->solution << "\"" << endl << flush;

	//char *MYSOL = solver->solution;	



	//char *MYSOL="U R R D U U L D L L U L L D R R R R L D D R U R U D L L U R";
    //char *MYSOL="FAIL";
	
    cout << endl << "Answer from server: ";
	n = write(sockfd,MYSOL,strlen(MYSOL));
    if (n < 0) 
         error("ERROR writing to socket");
    memset(buffer, 0, BUFFERSIZE);
    n = read(sockfd,buffer,BUFFERSIZE-1);
	printf("%s\n",buffer);
    return 0;
}
