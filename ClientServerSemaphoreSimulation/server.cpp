//Jacob Kapusta
//SID: 1181827
//COSC 4330
//Assignment 1

#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

class semaphore
{
	public:
		list<int> procqueue;
		int sem_id;
		int size;
		int* slot;
		void create_semaphore(int id, int setup)
		{
			sem_id = id;
			size = setup;
			slot = new int[setup];
			procqueue.clear();
			for(int i = 0; i<setup; i++)
			{
				slot[i] = -1;
			}
		}
		int getSemID()
		{
			return sem_id;
		}
};

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
	char buffer[256];

	int sockfd, newsockfd, n;
    socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	//Create new socket.
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
		error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(10599);
	 
	 //Bind socket.
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
             
     //Listen for incoming client requests.
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
		error("ERROR on accept");
	 
	 //All communications with client.
	 int no_semaphores = 0;
	 semaphore tempsem;
	 list<semaphore> open_semaphores;
	 char command[10];
	 char type[10];
	 int procid;
	 int sem_id;
	 int semslots;
	 bool exists = false;
	 bool freeslot = false;
	 bool found = false;
	 int tempint;
	 for(;;)
	 {
		bzero(buffer, 256);
        n = read(newsockfd,buffer,255);
		if(buffer[0] == 'c')
		{
			sscanf(buffer,"%s %d\n",command,&semslots); //parse buffer
			tempsem.create_semaphore(no_semaphores, semslots); //build the semaphore
			open_semaphores.push_front(tempsem); //create the semaphore
			tempsem = open_semaphores.front();
			bzero(buffer, 256); //clear the buffer
			snprintf(buffer,256,"%d\n",no_semaphores); //create message to send with sem_id
			no_semaphores++; //increment number of semaphores
			n = write(newsockfd,buffer,255); //send the sem_id
		}
		if(buffer[0] == 'k')
		{
			bzero(buffer, 256);
			snprintf(buffer,256,"success");
			n = write(newsockfd,buffer,255);
			bzero(buffer, 256);
			close(newsockfd);
			close(sockfd);
			return 0;
		}
		if(buffer[0] == 'p')
		{
			sscanf(buffer,"%s %s %d %d\n",command,type,&procid,&sem_id); //parse buffer
			if(strcmp(type,"i") == 0)
			{
				bzero(buffer, 256);
				for(list<semaphore>::iterator it=open_semaphores.begin(); it != open_semaphores.end(); ++it)
				{
					tempsem = *it;
					if(tempsem.sem_id == sem_id) exists = true;
				}
				if(exists == false)
				{
					bzero(buffer, 256);
					snprintf(buffer,256,"nonexistent");
					n = write(newsockfd,buffer,255);
				}
				if(exists == true)
				{
					for(list<semaphore>::iterator it=open_semaphores.begin(); it != open_semaphores.end(); ++it)
					{
						tempsem = *it;
						if(tempsem.sem_id == sem_id)
						{
							for(int i = 0; i<tempsem.size; i++)
							{
								if(tempsem.slot[i] == -1)
								{
									tempsem.slot[i] = procid;
									*it = tempsem;
									snprintf(buffer,256,"accepted");
									n = write(newsockfd,buffer,255);
									freeslot = true;
									break;
								}
							}
							if(freeslot == false)
							{
								tempsem.procqueue.push_back(procid);
								*it = tempsem;
								snprintf(buffer,256,"enqueued");
								n = write(newsockfd,buffer,255);
							}
						}
					}
					freeslot = false;
					exists = false;
				}
			}
			if(strcmp(type,"r") == 0)
			{
				bzero(buffer, 256);
				for(list<semaphore>::iterator it=open_semaphores.begin(); it != open_semaphores.end(); ++it)
				{
					tempsem = *it;
					if(tempsem.sem_id == sem_id)
					{
						for(list<int>::iterator it2=tempsem.procqueue.begin(); it2 != tempsem.procqueue.end(); ++it2)
						{
							tempint = *it2;
							if(tempint == procid) found = true;
						}
					}
				}
				if(found == true)
				{
					snprintf(buffer,256,"enqueued");
					n = write(newsockfd,buffer,255);
				}
				if(found == false)
				{
					snprintf(buffer,256,"accepted");
					n = write(newsockfd,buffer,255);
				}
			}
		}
		if(buffer[0] == 'v')
		{
			sscanf(buffer,"%s %d %d\n",command,&procid,&sem_id); //parse buffer
			for(list<semaphore>::iterator it=open_semaphores.begin(); it != open_semaphores.end(); ++it)
			{
				tempsem = *it;
				if(tempsem.sem_id == sem_id)
				{
					for(int i = 0; i<tempsem.size; i++)
					{
						if(tempsem.slot[i] == procid)
						{
							tempsem.slot[i] = -1;
							if(!tempsem.procqueue.empty())
							{
								tempsem.slot[i] = tempsem.procqueue.front();
								tempsem.procqueue.pop_front();
							}
							found = true;
							*it = tempsem;
							break;
						}
					}
				}
			}
			bzero(buffer,256);
			if(found == true) snprintf(buffer,256,"accepted");
			if(found == false) snprintf(buffer,256,"nonexistent");
			found = false;
			n = write(newsockfd,buffer,255);
		}
		if(buffer[0] == 'd')
		{
			sscanf(buffer,"%s %d\n",command,&sem_id); //parse buffer
			
			if(open_semaphores.size() == 1)
			{
				if(open_semaphores.front().sem_id == sem_id)
				{
					open_semaphores.pop_front();
					found = true;
				}
			}
			
			else
			{
				for(list<semaphore>::iterator it=open_semaphores.begin(); it != open_semaphores.end(); ++it)
				{
					tempsem = *it;
					if(tempsem.sem_id == sem_id)
					{
						open_semaphores.erase(it);
						found = true;
						break;
					}
				}
			}
			bzero(buffer,256);
			if(found == true) snprintf(buffer,256,"accepted");
			if(found == false) snprintf(buffer,256,"nonexistent");
			found = false;
			n = write(newsockfd,buffer,255);
		}
	 }
	 return 0;
}