//Jacob Kapusta 1181827 COSC 4377

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//Function to terminate program if anything goes wrong.
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    
     char *Dmssgs = argv[2];
     
     //Attributes to track number of items and production rates.
     int rate_SPCR = 30;
     int curr_SPCR = 0;
     double time_SPCR = 0.0;
     int rate_MTCL = 22;
     int curr_MTCL = 0;
     double time_MTCL = 0.0;
     int rate_ENGF = 41;
     int curr_ENGF = 0;
     double time_ENGF = 0.0;
     int rate_ENGA = 2;
     int curr_ENGA = 0;
     double time_ENGA = 0.0;
     int rate_ACMP = 50;
     int curr_ACMP = 0;
     double time_ACMP = 0.0;
     
     //Attributes for string parsing.
     char *command, *product, *quantity;
    
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     char temp[256];
     bool cont = true;
     int i;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     //Create new socket.
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     //Bind socket.
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
             
     //Listen for incoming client requests.
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
          
     //Loop for client command input.      
     while(cont == true)
     {
        //String parsing portion.
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        memset(temp,0,sizeof temp);
        if (n < 0) error("ERROR reading from socket");
        if(strcmp(Dmssgs,"yes") == 0 && buffer[0] != 'd')
    	    printf("%s %s\n","client: ",buffer);
    	if(buffer[0] == 'D')
    	{
    	    bzero(temp,256);
    	}
    	//RST SPCR
    	else if(buffer[0] == 'R' && buffer[4] == 'S')
    	{
    	    command = strtok(buffer," ");
    	    product = strtok(NULL, " ");
    	    curr_SPCR = 0;
    	    time_SPCR = 0.0;
    	    bzero(temp,256);
    	    snprintf(temp,256,"server: %s %s : %d : %.2f",command,product,curr_SPCR,time_SPCR);
    	}
    	//RST MTCL
    	else if(buffer[0] == 'R' && buffer[4] == 'M')
    	{
    	    command = strtok(buffer," ");
    	    product = strtok(NULL," ");
    	    curr_MTCL = 0;
    	    time_MTCL = 0.0;
    	    bzero(temp,256);
    	    snprintf(temp,256,"server: %s %s : %d : %.2f",command,product,curr_MTCL,time_MTCL);
    	}
    	//RST ENGF
    	else if(buffer[0] == 'R' && buffer[4] == 'E' && buffer[7] == 'F')
    	{
    	    command = strtok(buffer," ");
    	    product = strtok(NULL," ");
    	    curr_ENGF = 0;
    	    time_ENGF = 0.0;
    	    bzero(temp,256);
    	    snprintf(temp,256,"server: %s %s : %d : %.2f",command,product,curr_ENGF,time_ENGF);
    	}
    	//RST ENGA
    	else if(buffer[0] == 'R' && buffer[4] == 'E' && buffer[7] == 'A')
    	{
    	    command = strtok(buffer," ");
    	    product = strtok(NULL," ");
    	    curr_ENGA = 0;
    	    time_ENGA = 0.0;
    	    bzero(temp,256);
    	    snprintf(temp,256,"server: %s %s : %d : %.2f",command,product,curr_ENGA,time_ENGA);
    	}        
    	//RST ACMP    
    	else if(buffer[0] == 'R' && buffer[4] == 'A')
    	{
    	    command = strtok(buffer," ");
    	    product = strtok(NULL," ");
    	    curr_ACMP = 0;
    	    time_ACMP = 0.0;
    	    bzero(temp,256);
    	    snprintf(temp,256,"server: %s %s : %d : %.2f",command,product,curr_ACMP,time_ACMP);
    	}    
    	//ADD and SUB
    	else{
    	    command = strtok(buffer, " ");
    	    product = strtok(NULL, " ");
    	    quantity = strtok(NULL, " ");
    
        //Addition routes.
        if(strcmp(command,"ADD") == 0)
        {
    	    if(strcmp(product,"SPCR") == 0)
    	    {
    		curr_SPCR += atoi(quantity);
    		time_SPCR = (double)curr_SPCR/(double)rate_SPCR;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_SPCR,time_SPCR);
    	    }
    	    if(strcmp(product,"MTCL") == 0)
    	    {
    		curr_MTCL += atoi(quantity);
    		time_MTCL = (double)curr_MTCL/(double)rate_MTCL;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_MTCL,time_MTCL);
    	    }
    	    if(strcmp(product,"ENGF") == 0)
    	    {
    		curr_ENGF += atoi(quantity);
    		time_ENGF = (double)curr_ENGF/(double)rate_ENGF;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_ENGF,time_ENGF);
    	    }
    	    if(strcmp(product,"ENGA") == 0)
    	    {
    		curr_ENGA += atoi(quantity);
    		time_ENGA = (double)curr_ENGA/(double)rate_ENGA;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_ENGA,time_ENGA);
    	    }
    	    if(strcmp(product,"ACMP") == 0)
    	    {
    		curr_ACMP += atoi(quantity);
    		time_ACMP = (double)curr_ACMP/(double)rate_ACMP;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_ACMP,time_ACMP);
    	    }
    	}
    	//Subtraction routes.
    	if(strcmp(command,"SUB") == 0)
    	{
    	    if(strcmp(product,"SPCR") == 0)
    	    {
    		curr_SPCR -= atoi(quantity);
    		time_SPCR = (double)curr_SPCR/(double)rate_SPCR;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_SPCR,time_SPCR);
    	    }
    	    if(strcmp(product,"MTCL") == 0)
    	    {
    		curr_MTCL -= atoi(quantity);
    		time_MTCL = (double)curr_MTCL/(double)rate_MTCL;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_MTCL,time_MTCL);
    	    }
    	    if(strcmp(product,"ENGF") == 0)
    	    {
    		curr_ENGF -= atoi(quantity);
    		time_ENGF = (double)curr_ENGF/(double)rate_ENGF;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_ENGF,time_ENGF);
    	    }
    	    if(strcmp(product,"ENGA") == 0)
    	    {
    		curr_ENGA -= atoi(quantity);
    		time_ENGA = (double)curr_ENGA/(double)rate_ENGA;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_ENGA,time_ENGA);
    	    }
    	    if(strcmp(product,"ACMP") == 0)
    	    {
    		curr_ACMP -= atoi(quantity);
    		time_ACMP = (double)curr_ACMP/(double)rate_ACMP;
    		bzero(temp,256);
    		snprintf(temp,256,"server: %s %s %s : %d : %.2f",command,product,quantity,curr_ACMP,time_ACMP);
    	    }
    	}
    	}
    	
	//This line sends timings.
        n = write(newsockfd,temp,255);
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,256);
        bzero(temp,256);
        //Figure out if there is another command.
        n = read(newsockfd,buffer,255);
        i = atoi(&buffer[0]);
        if(i == 2) cont = false;
     }
     
     //Terminate program.
     close(newsockfd);
     close(sockfd);
     return 0; 
}
