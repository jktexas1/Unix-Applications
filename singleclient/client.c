//Jacob Kapusta 1181827 COSC 4377

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//Function to terminate if something goes wrong.
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char temp[256];
    char *curr_file = "N/A";
    char *done = "done";
    char command[256] = "command";
    if(argc == 5) curr_file = argv[4];
    char *filename = curr_file;
    char *Dmssgs = argv[3];
    bool cont = true;
    int i;
    int compareLim = 255;
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    
    //Create new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
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
    
    //Connect to server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    //Loop for case user input.
    if(argc == 4){
	while(cont == true)
	{
		printf("Please give your command (Example: ADD SPCR 60): ");
		bzero(buffer,256);
    		fgets(buffer,255,stdin);
		n = write(sockfd,buffer,strlen(buffer));
		if(strcmp(Dmssgs,"yes") == 0)
		    printf("%s %s\n","client: ",buffer);
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		printf("%s\n",buffer);
		printf("Another command?(yes: 1, no: 2)");
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		n = write(sockfd,buffer,255);
		i = atoi(&buffer[0]);
		if(i == 2) cont = false;
	}
    }
    
    //Loop for case textfile
    if(argc == 5){
	//static const char filename[] = argv[4];
	FILE *file = fopen(filename,"r");
	if(file != NULL)
	{
	    char line[256];
	    bzero(buffer,256);
	    while(fgets(line,256,file) != NULL)
	    {
		n = write(sockfd,line,255);
		if(strcmp(Dmssgs,"yes") == 0)
		    printf("%s %s\n","client: ",line);
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		printf("%s\n",buffer);
		bzero(buffer,256);
		buffer[0] = '1';
		n = write(sockfd,buffer,255);	   
	     }
	}
	//This portion does one final communication to stop sending commands.
	n = write(sockfd,done,5);
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	bzero(buffer,256);
	buffer[0] = '2';
	n = write(sockfd,buffer,255);
    }
    //Close connection.
    close(sockfd);
    return 0;
}
