//Jacob Kapusta
//SID: 1181827
//COSC 4330
//Assignment 1

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

// you can add all the includes you need but
// cannot modify the main program

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int sem_create(int initial_value);
int kill_server();
int sem_P(int sem_id);
int sem_V(int sem_id);
int sem_destroy(int sem_id);
void get_connection();

char buffer[256];
int sockfd = -1;
int portno;

int main() {
    int sem1, mutex; //semaphores
    int pid; // process ID
    if ((sem1 = sem_create(3)) < 0) {
         printf("Cannot create semaphore sem1.\n");
         _exit(1);
    } 
    if ((mutex = sem_create(1)) < 0) {
         _exit(2);
         printf("Cannot create semaphore mutex.\n");
    } 
    // basic checks
    sem_P(sem1);
    sem_P(sem1);
    sem_P(sem1);
    printf("Completed three P() operations on sem1.\n");
    sem_V(sem1);
    sem_V(sem1);
    sem_V(sem1);
    printf("Completed three V() operations on sem1.\n");
    if (sem_P(10) >= 0) {
        printf("Server accepted a call to a non-existent semaphore!\n");
    }
    
    if ((pid = fork()) == 0) {
        // child process
        printf("Child process requests the mutex.\n");
        sem_P(mutex);
        printf("Child process got the mutex.\n");
        sleep(10);
        printf("Child process wants to release the mutex.\n");
        sem_V(mutex);
        printf("Child process released the mutex.\n");
        _exit(0);
    }
    sleep(2);
    printf("Parent process requests the mutex.\n");
    sem_P(mutex);
    printf("Parent process got the mutex.\n");
    sleep(10);
    printf("Parent process wants to release the mutex.\n");
    sem_V(mutex);
    printf("Parent process released the mutex.\n");

    // Ending
    sem_destroy(sem1);
    if (sem_P(sem1) >= 0) {
        printf("Semaphore sem1 was not properly destroyed!\n");
    }
    sem_destroy(mutex);
    if (sem_P(mutex) >= 0) {
        printf("Semaphore mutex was not properly destroyed!\n");
    }
    kill_server();
} // main

int sem_create(int initial_value) {
	if(sockfd == -1)
	{
		get_connection();
	}
	int n;
	int id;
	bzero(buffer, 256);
	snprintf(buffer,256,"c %d\n", initial_value);
	n = write(sockfd,buffer,strlen(buffer));
	bzero(buffer, 256);
	n = read(sockfd,buffer,255);
	sscanf(buffer,"%d\n",&id);
	if(id != -1)
	{
		return id;
	}
	else
	{
		return -1;
	}
	cout << buffer << endl;
}

int kill_server() {
	if(sockfd == -1)
	{
		get_connection();
	}
	int n;
	bzero(buffer, 256);
	buffer[0] = 'k';
	n = write(sockfd,buffer,strlen(buffer));
	bzero(buffer, 256);
	n = read(sockfd,buffer,255);
	if(strcmp(buffer, "success") == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int sem_P(int sem_id) {
	if(sockfd == -1)
	{
		get_connection();
	}
	int n, procid;
	bool success = false;
	procid = getpid();
	bzero(buffer, 256);
	snprintf(buffer,256,"p i %d %d", procid, sem_id);
	n = write(sockfd,buffer,strlen(buffer));
	bzero(buffer, 256);
	n = read(sockfd,buffer,255);
	if(strcmp(buffer, "nonexistent") == 0)
	{
		return -1;
	}
	if(strcmp(buffer, "accepted") == 0)
	{
		success = true;
		return 0;
	}
	while(success == false)
	{
		bzero(buffer, 256);
		snprintf(buffer,256,"p r %d %d", procid, sem_id);
		n = write(sockfd,buffer,strlen(buffer));
		bzero(buffer, 256);
		n = read(sockfd,buffer,255);
		if(strcmp(buffer, "accepted") == 0)
		{
			success = true;
			return 1;
		}
	}
}	

int sem_V(int sem_id) {
	if(sockfd == -1)
	{
		get_connection();
	}
	int n, procid;
	procid = getpid();
	bzero(buffer, 256);
	snprintf(buffer,256,"v %d %d\n",procid, sem_id);
	n = write(sockfd,buffer,strlen(buffer));
	bzero(buffer, 256);
	n = read(sockfd,buffer,255);
	if(strcmp(buffer, "accepted") == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int sem_destroy(int sem_id) {
	if(sockfd == -1)
	{
		get_connection();
	}
	int n;
	bzero(buffer, 256);
	snprintf(buffer,256,"d %d\n",sem_id);
	n = write(sockfd,buffer,strlen(buffer));
	bzero(buffer, 256);
	n = read(sockfd,buffer,255);
	if(strcmp(buffer, "accepted") == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void get_connection()
{
	struct sockaddr_in serv_addr;
    struct hostent *server;
	
	portno = 10599;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("bayou");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
}
