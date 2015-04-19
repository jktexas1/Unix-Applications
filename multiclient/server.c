#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include <fcntl.h>

#define PORT 		4001
#define MAX_clients	5

/* Structure for managing users */
struct priority_keeper
{
	char name[15];
	int priority;
	bool entered_SPCR;
	int pos_SPCR;
	bool entered_MTCL;
	int pos_MTCL;
	bool entered_ENGF;
	int pos_ENGF;
	bool entered_ENGA;
	int pos_ENGA;
	bool entered_ACMP;
	int pos_ACMP;
};

/* Structure for managing information in queue nodes */
struct queue_info
{
	char name[15];
	int quantity;
	int priority;
};

bool enforced = false;

struct queue_info SPCR_Q[1000];
struct queue_info MTCL_Q[1000];
struct queue_info ENGF_Q[1000];
struct queue_info ENGA_Q[1000];
struct queue_info ACMP_Q[1000];

int SPCR_SIZE = 0;
int MTCL_SIZE = 0;
int ENGF_SIZE = 0;
int ENGA_SIZE = 0;
int ACMP_SIZE = 0;

void prioritize(struct queue_info CURR_Q[1000], int size)
{
	int i, j;
	struct queue_info temp;

	//Sort queue based on priority
	for(i=0;i<size-1;i++)
	{
		for(j=i+1;j<size;j++)
		{
			if(CURR_Q[i].priority > CURR_Q[j].priority)
			{
				//Copy Lesser priority into temp
				strcpy(temp.name,CURR_Q[i].name);
				temp.quantity = CURR_Q[i].quantity;
				temp.priority = CURR_Q[i].priority;
				//Move Greater priority into prior position
				strcpy(CURR_Q[i].name,CURR_Q[j].name);
				CURR_Q[i].quantity = CURR_Q[j].quantity;
				CURR_Q[i].priority = CURR_Q[j].priority;
				//Move Temp into j
				strcpy(CURR_Q[j].name,temp.name);
				CURR_Q[j].quantity = temp.quantity;
				CURR_Q[j].priority = temp.priority;
			}
		}
	}
}

/* Function to find a user's position in a queue */
int find_pos(struct priority_keeper list[5], struct queue_info CURR_Q[1000], int priority, int size)
{
	int i, j, pos;
	bool found = false;
	//Find user position
				for(i=0;i<size;i++)
				{
					for(j=0;j<4;j++) //Testing
					{
						if(list[priority-1].name[j] == CURR_Q[i].name[j]) found = true;
						else if(list[priority-1].name[j] != CURR_Q[i].name[j]) 
						{
							found = false;
							break;
						}
					}
					if(found == true)
					{
						pos = i;
						return pos;
						break;
					}
				}
}

/* Function to enqueue a command */
void enqueue(struct priority_keeper list[5], char user[], char command[], char product[], int quantity, int priority)
{
	bool found = false;
	int i, j, k;
	int pos;
	int quantity_hold = quantity;
	if(strcmp(command,"ADD") == 0 || strcmp(command,"SUB") == 0)
	{
		if(strcmp(command,"SUB") == 0) quantity *= -1;
		//Enqueue
		if(strcmp(product,"SPCR") == 0){
			if(list[priority-1].entered_SPCR == true)
			{
				pos = find_pos(list, SPCR_Q, priority, SPCR_SIZE);
				list[priority-1].pos_SPCR = pos;
				if(strcmp(command,"SUB") == 0 && SPCR_Q[pos].quantity < quantity_hold) quantity = 0; //Set quantity to 0 if subtraction results in a negative
				SPCR_Q[pos].quantity = SPCR_Q[pos].quantity + quantity;
			}
			else if(list[priority-1].entered_SPCR == false)
			{
				if(strcmp(command,"ADD") == 0) //Only able to add if not entered queue
				{
					strcpy(SPCR_Q[SPCR_SIZE].name,user);
					SPCR_Q[SPCR_SIZE].quantity = quantity;
					SPCR_Q[SPCR_SIZE].priority = priority;
					SPCR_SIZE++;
					prioritize(SPCR_Q, SPCR_SIZE);
					list[priority-1].entered_SPCR = true;
					pos = find_pos(list, SPCR_Q, priority, SPCR_SIZE);
					list[priority-1].pos_SPCR = pos;
				}
			}
		}else if(strcmp(product,"MTCL") == 0){
			if(list[priority-1].entered_MTCL == true)
			{
				pos = find_pos(list, MTCL_Q, priority, MTCL_SIZE);
				list[priority-1].pos_MTCL = pos;
				if(strcmp(command,"SUB") == 0 && MTCL_Q[pos].quantity < quantity_hold) quantity = 0; //Set quantity to 0 if subtraction results in a negative
				MTCL_Q[pos].quantity = MTCL_Q[pos].quantity + quantity;
			}
			else if(list[priority-1].entered_MTCL == false)
			{
				if(strcmp(command,"ADD") == 0) //Only able to add if not entered queue
				{
					strcpy(MTCL_Q[MTCL_SIZE].name,user);
					MTCL_Q[MTCL_SIZE].quantity = quantity;
					MTCL_Q[MTCL_SIZE].priority = priority;
					MTCL_SIZE++;
					prioritize(MTCL_Q, MTCL_SIZE);
					list[priority-1].entered_MTCL = true;
					pos = find_pos(list, MTCL_Q, priority, MTCL_SIZE);
					list[priority-1].pos_MTCL = pos;
				}
			}
		}else if(strcmp(product,"ENGF") == 0){
			if(list[priority-1].entered_ENGF == true)
			{
				pos = find_pos(list, ENGF_Q, priority, ENGF_SIZE);
				list[priority-1].pos_ENGF = pos;
				if(strcmp(command,"SUB") == 0 && ENGF_Q[pos].quantity < quantity_hold) quantity = 0; //Set quantity to 0 if subtraction results in a negative
				ENGF_Q[pos].quantity = ENGF_Q[pos].quantity + quantity;
			}
			else if(list[priority-1].entered_ENGF == false)
			{
				if(strcmp(command,"ADD") == 0) //Only able to add if not entered queue
				{
					strcpy(ENGF_Q[ENGF_SIZE].name,user);
					ENGF_Q[ENGF_SIZE].quantity = quantity;
					ENGF_Q[ENGF_SIZE].priority = priority;
					ENGF_SIZE++;
					prioritize(ENGF_Q, ENGF_SIZE);
					list[priority-1].entered_ENGF = true;
					pos = find_pos(list, ENGF_Q, priority, ENGF_SIZE);
					list[priority-1].pos_ENGF = pos;
				}
			}
		}else if(strcmp(product,"ENGA") == 0){
			if(list[priority-1].entered_ENGA == true)
			{
				pos = find_pos(list, ENGA_Q, priority, ENGA_SIZE);
				list[priority-1].pos_ENGA = pos;
				if(strcmp(command,"SUB") == 0 && ENGA_Q[pos].quantity < quantity_hold) quantity = 0; //Set quantity to 0 if subtraction results in a negative
				ENGA_Q[pos].quantity = ENGA_Q[pos].quantity + quantity;
			}
			else if(list[priority-1].entered_ENGA == false)
			{
				if(strcmp(command,"ADD") == 0) //Only able to add if not entered queue
				{
					strcpy(ENGA_Q[ENGA_SIZE].name,user);
					ENGA_Q[ENGA_SIZE].quantity = quantity;
					ENGA_Q[ENGA_SIZE].priority = priority;
					ENGA_SIZE++;
					prioritize(ENGA_Q, ENGA_SIZE);
					list[priority-1].entered_ENGA = true;
					pos = find_pos(list, ENGA_Q, priority, ENGA_SIZE);
					list[priority-1].pos_ENGA = pos;
				}
			}
		}else if(strcmp(product,"ACMP") == 0){
			if(list[priority-1].entered_ACMP == true)
			{
				pos = find_pos(list, ACMP_Q, priority, ACMP_SIZE);
				list[priority-1].pos_ACMP = pos;
				if(strcmp(command,"SUB") == 0 && ACMP_Q[pos].quantity < quantity_hold) quantity = 0; //Set quantity to 0 if subtraction results in a negative
				ACMP_Q[pos].quantity = ACMP_Q[pos].quantity + quantity;
			}
			else if(list[priority-1].entered_ACMP == false)
			{
				if(strcmp(command,"ADD") == 0) //Only able to add if not entered queue
				{
					strcpy(ACMP_Q[ACMP_SIZE].name,user);
					ACMP_Q[ACMP_SIZE].quantity = quantity;
					ACMP_Q[ACMP_SIZE].priority = priority;
					ACMP_SIZE++;
					prioritize(ACMP_Q, ACMP_SIZE);
					list[priority-1].entered_ACMP = true;
					pos = find_pos(list, ACMP_Q, priority, ACMP_SIZE);
					list[priority-1].pos_ACMP = pos;
				}
			}
		}
	}
}

/* Function to reset user's entries in a queue */
void reset(char user[], int priority, struct priority_keeper list[5], struct queue_info CURR_Q[1000], int size)
{
	int pos, i;
	pos = find_pos(list, CURR_Q, priority, size);
	for(i=pos;i<size;i++)
	{
		strcpy(CURR_Q[i].name, CURR_Q[i+1].name);
		CURR_Q[i].quantity = CURR_Q[i+1].quantity;
		CURR_Q[i].priority = CURR_Q[i+1].priority;
	}
}

/* Current totals for each product */
int  spcr_quantity;
int  mtcl_quantity;
int  engf_quantity;
int  enga_quantity;
int  acmp_quantity;

bool debug_message;

/* Function to turn in_message into out_message */
int process_command(char in_message[], char out_message[], struct priority_keeper list[5]) 
{
  int priority_curr, i, j;
  priority_curr = 0;
  int pos;

  bool found = false;

  char user[15];
  char command[3];
  char product[4];

  int quantity;
  int quantity_hold = 0;
  int quantity_hold2 = 0;
  int product_quantity;
  float rate = 0;
  float rate_hold = 0;

  int position;

  /* Assembly rates of units per hour */
  const float spcr_rate = 30;
  const float mtcl_rate = 22;
  const float engf_rate = 41;
  const float enga_rate = 2;
  const float acmp_rate = 50;

  /* Separate the command, product and quantity values from input message */
  sscanf(in_message,"%s %s %s %d\n",user,command,product,&quantity);

  
  /* Get user's priority number */
  for(i=0;i<5;i++)
  {
	  for(j=0;j<4;j++)
	  {
		if(list[i].name[j] == user[j])
		{
			found = true;
		}
		else if(list[i].name[j] != user[j])
		{
			found = false;
		}
		if(found == false) break;
		else continue;
	  }
	  if(found == true){
		  priority_curr = list[i].priority;
		  break;
	  }
  }

  /* Print the received values i.e. the message */		
  if (debug_message) {
      if (strcmp(command,"RST")==0)
          printf("[Server] We have received: %s %s\n",command,product);
      else if (strcmp(command,"END")==0)
          printf("[Server] We have received: %s\n",command);
      else
          printf("[Server] We have received: %s %s %d\n",command,product,quantity);
  }

  /* Enqueue current command */
  if(strcmp(command,"ADD") == 0 || strcmp(command,"SUB") == 0) 
  {
	  enqueue(list, user, command, product, quantity, priority_curr);
  }

  /* Construct out message */
  if(strcmp(command,"SUB") == 0)
  {
	  if(strcmp(product,"SPCR") == 0)
	  {
		  if(list[priority_curr-1].entered_SPCR == false) sprintf(out_message,"You have no SPCR to subtract from");
	  }
	  if(strcmp(product,"MTCL") == 0)
	  {
		  if(list[priority_curr-1].entered_MTCL == false) sprintf(out_message,"You have no MTCL to subtract from");
	  }
	  if(strcmp(product,"ENGF") == 0)
	  {
		  if(list[priority_curr-1].entered_ENGF == false) sprintf(out_message,"You have no ENGF to subtract from");
	  }
	  if(strcmp(product,"ENGA") == 0)
	  {
		  if(list[priority_curr-1].entered_ENGA == false) sprintf(out_message,"You have no ENGA to subtract from");
	  }
	  if (strcmp(product,"ACMP")==0)
	  {
		  if(list[priority_curr-1].entered_ACMP == false) sprintf(out_message,"You have no ACMP to subtract from");
	  }
  }
  if(strcmp(command,"ADD") == 0 || strcmp(command,"SUB") == 0)
  {
	  if(strcmp(product,"SPCR") == 0 && list[priority_curr-1].entered_SPCR == true)
	  {
		  pos = find_pos(list, SPCR_Q, priority_curr, SPCR_SIZE);
		  for(i=0;i<pos+1;i++) quantity_hold += SPCR_Q[i].quantity;
		  rate_hold = quantity_hold / spcr_rate;
		  quantity_hold2 = SPCR_Q[pos].quantity;
		  sprintf(out_message,"%s %s %d : %d : %4.2f : %d",command,product,quantity,quantity_hold2,rate_hold,pos+1);
	  }
	  if(strcmp(product,"MTCL") == 0 && list[priority_curr-1].entered_MTCL == true)
	  {
		  pos = find_pos(list, MTCL_Q, priority_curr, MTCL_SIZE);
		  for(i=0;i<pos+1;i++) quantity_hold += MTCL_Q[i].quantity;
		  rate_hold = quantity_hold / mtcl_rate;
		  quantity_hold2 = MTCL_Q[pos].quantity;
		  sprintf(out_message,"%s %s %d : %d : %4.2f : %d",command,product,quantity,quantity_hold2,rate_hold,pos+1);
	  }
	  if(strcmp(product,"ENGF") == 0 && list[priority_curr-1].entered_ENGF == true)
	  {
		  pos = find_pos(list, ENGF_Q, priority_curr, ENGF_SIZE);
		  for(i=0;i<pos+1;i++) quantity_hold += ENGF_Q[i].quantity;
		  rate_hold = quantity_hold / engf_rate;
		  quantity_hold2 = ENGF_Q[pos].quantity;
		  sprintf(out_message,"%s %s %d : %d : %4.2f : %d",command,product,quantity,quantity_hold2,rate_hold,pos+1);
	  }
	  if(strcmp(product,"ENGA") == 0 && list[priority_curr-1].entered_ENGA == true)
	  {
		  pos = find_pos(list, ENGA_Q, priority_curr, ENGA_SIZE);
		  for(i=0;i<pos+1;i++) quantity_hold += ENGA_Q[i].quantity;
		  rate_hold = quantity_hold / enga_rate;
		  quantity_hold2 = ENGA_Q[pos].quantity;
		  sprintf(out_message,"%s %s %d : %d : %4.2f : %d",command,product,quantity,quantity_hold2,rate_hold,pos+1);
	  }
	  if (strcmp(product,"ACMP")==0 && list[priority_curr-1].entered_ACMP == true)
	  {
		  pos = find_pos(list, ACMP_Q, priority_curr, ACMP_SIZE);
		  for(i=0;i<pos+1;i++) quantity_hold += ACMP_Q[i].quantity;
		  rate_hold = quantity_hold / acmp_rate;
		  quantity_hold2 = ACMP_Q[pos].quantity;
		  sprintf(out_message,"%s %s %d : %d : %4.2f : %d",command,product,quantity,quantity_hold2,rate_hold,pos+1);
	  }
  }
  
  /* If the command is RST */
  if(strcmp(command,"RST") == 0)
  {
	  if (strcmp(product,"SPCR")==0)
	  {
		  if(list[priority_curr-1].entered_SPCR == true)
		  {
			  reset(user, priority_curr, list, SPCR_Q, SPCR_SIZE);
			  list[priority_curr-1].entered_SPCR = false;
			  sprintf(out_message,"%s","SPCR entries removed");
			  SPCR_SIZE--;
		  }
		  else
		  {
			  sprintf(out_message,"You have no SPCR to reset");
		  }
	  }
	  if (strcmp(product,"MTCL")==0)
	  {
		  if(list[priority_curr-1].entered_MTCL == true)
		  {
			  reset(user, priority_curr, list, MTCL_Q, MTCL_SIZE);
			  list[priority_curr-1].entered_MTCL = false;
			  sprintf(out_message,"%s","MTCL entries removed");
			  MTCL_SIZE--;
		  }
		  else
		  {
			  sprintf(out_message,"You have no MTCL to reset");
		  }
	  }
	  if (strcmp(product,"ENGF")==0)
	  {
		  if(list[priority_curr-1].entered_ENGF == true)
		  {
			  reset(user, priority_curr, list, ENGF_Q, ENGF_SIZE);
			  list[priority_curr-1].entered_ENGF = false;
			  sprintf(out_message,"%s","ENGF entries removed");
			  ENGF_SIZE--;
		  }
		  else
		  {
			  sprintf(out_message,"You have no ENGF to reset");
		  }
	  }
	  if (strcmp(product,"ENGA")==0)
	  {
		  if(list[priority_curr-1].entered_ENGA == true)
		  {
			  reset(user, priority_curr, list, ENGA_Q, ENGA_SIZE);
			  list[priority_curr-1].entered_ENGA = false;
			  sprintf(out_message,"%s","ENGA entries removed");
			  ENGA_SIZE--;
		  }
		  else
		  {
			  sprintf(out_message,"You have no ENGA to reset");
		  }
	  }
	  if (strcmp(product,"ACMP")==0)
	  {
		  if(list[priority_curr-1].entered_ACMP == true)
		  {
			  reset(user, priority_curr, list, ACMP_Q, ACMP_SIZE);
			  list[priority_curr-1].entered_ACMP = false;
			  sprintf(out_message,"%s","ACMP entries removed");
			  ACMP_SIZE--;
		  }
		  else
		  {
			  sprintf(out_message,"You have no ACMP to reset");
		  }
	  }
  }
	
  /* Do nothing if command is END */
  if (strcmp(command,"END") == 0) return 1;

  return 0;
}

void setnonblocking(tmp_sock)
	int tmp_sock;
{
	int opts;

	opts = fcntl(tmp_sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(1);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(tmp_sock,F_SETFL,opts) < 0) {
		perror("fcntl(F_SETFL)");
		exit(1);
	}
}

int main(argc, argv)
	int argc;
	char *argv[];
{
	int 	 sd, cc, fromlen, tolen;
	int	 highsd, incoming_sd;
	int	 sd_list[MAX_clients];
	int 	 port_number;     /* port number for receiving data */
	int      reuse_addr = 1;  /* Used so we can re-bind to our port
				     while a previous connection is still
				     in TIME_WAIT state. */
	int 	 addrlen;
	struct   sockaddr_in sin;
	struct   sockaddr_in pin;
        int i, tmp_i, tmp_j;
		int j;
	int recvd;
	int structlength;

	char  in_message[100];  // Input message from the client
    char  out_message[100]; // Output message to send to the client

	//char buf[100];
	char tmp_str[100];

	/* NEW -- added for select() */
	int rc;
	struct timeval tv;
	fd_set rfds;

	if (argc < 2) {
		port_number = PORT;
	}
	else {
		port_number = atoi(argv[1]);
	}
	printf ("%s started: listening on port %d\n", argv[0], port_number);

	for (i = 0; i < MAX_clients; i++)
		sd_list[i] = 0;
 
	/* Reset current assembly totals for each product */
    spcr_quantity=0;
    mtcl_quantity=0;
    engf_quantity=0;
    enga_quantity=0;
    acmp_quantity=0;

	/* Enforce queue priorities */
	int n = 0;
    //priority_keeper* priority_list = malloc(users * sizeof *priority_list);
	struct priority_keeper priority_list[5];
    FILE *in_file;
    in_file = fopen(argv[3],"r");
	char line[15];
	if (!in_file) {
           printf("\nFile: %s was not found\n",in_file);
	   return -1; }
	while(fgets(line,15,in_file))
	{
		strcpy(priority_list[n].name, line);
		for(j=0;j<15;j++)
		{
			if(!isalpha(priority_list[n].name[j])) priority_list[n].name[j] = 'Z';
		}
		priority_list[n].priority = n+1;
		priority_list[n].entered_SPCR = false;
		priority_list[n].entered_MTCL = false;
		priority_list[n].entered_ENGF = false;
		priority_list[n].entered_ENGA = false;
		priority_list[n].entered_ACMP = false;
		n++;
	}	

	if (argc == 4) { // Expects: ~$ ./server [port] [debug_message] // = 3 prior to priority change
      
      port_number = atoi(argv[1]);
		
      if (strcmp(argv[2],"yes")==0)       debug_message=1;
      else if (strcmp(argv[2],"no")==0)   debug_message=0;
      else { printf("Invalid debug message\n"); return -2; }

      if (debug_message)
          printf("Input arguments: %s %d %d\n",argv[0],port_number,debug_message);

     } else { // This is in case we incorrectly specify input arguments
      printf("Invalid arguments. Expected arguments: %s [port] [debug]\n",argv[0]);
      return -1; }

	/* get an internet domain socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* So that we can re-bind to it without TIME_WAIT problems */
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
		sizeof(reuse_addr));

	/* Set socket to non-blocking with our setnonblocking routine */
	setnonblocking(sd);

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port_number);

	/* bind the socket to the port number */
	if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
		perror("bind");
		exit(1);
	}

	printf("After bind.\n");

	/* show that we are willing to listen */
	if (listen(sd, MAX_clients) == -1) {
		perror("listen");
		exit(1);
	}

	printf("After listen.\n");

	highsd = sd;

	/* clear input and output buffer */
        memset(&in_message, 0, sizeof(in_message));
		memset(&out_message, 0, sizeof(out_message));

	structlength = sizeof(sin);
	tmp_i = 0; tmp_j = 0;

	while(1) 
	{

	/* NEW -- added for select */
	  FD_ZERO(&rfds);
	  FD_SET(sd, &rfds);
	  FD_SET(fileno(stdin), &rfds);  /* monitoring keyboard input */ 
	  for (i = 0; i < MAX_clients; i++) {
		  if (sd_list[i] != 0) {
			  FD_SET(sd_list[i], &rfds);
			  if (sd_list[i] > highsd) {
				  highsd = sd_list[i];
			  }
		   }
	  }

	  /* timeout for select() */
	  tv.tv_sec = 0;
	  tv.tv_usec = 500000;

	  rc = select(FD_SETSIZE, &rfds, (fd_set *) NULL, (fd_set *) NULL, &tv);
	  if (rc == -1) {
	    perror("select");
	    exit(1);
	  } /* if */

	  if (rc == 0) {
		continue;
	  } /* if */

	  /* dealing with new connection */
      if (FD_ISSET(fileno(stdin), &rfds))
	  {
	    /* input from keyboard */
	    int result;
	    result = scanf("%s", tmp_str);
	    printf("keyboard input: %s\n", tmp_str);
	  }  /* if */
	  else {
		if (FD_ISSET(sd, &rfds)) 
		{
			if ((incoming_sd = accept(sd, (struct sockaddr *)  &pin, &addrlen)) == -1) 
			{
				perror("accept");
				exit(1);
			} /* if */

			printf ("After accept ...\n");

			setnonblocking(incoming_sd);
			for (i = 0; (i < MAX_clients) && (incoming_sd != -1); i++) {
				if (sd_list[i] == 0) {
					sd_list[i] = incoming_sd;
					break;
				} /* if */
			} /* for */
			if (i >= MAX_clients) {
				printf("No room for new connection (MAX: %d).\n",
						MAX_clients);
				close(incoming_sd);
			} /* if */
		} /* if */

		/* receiving data */
	    for (i = 0; i < MAX_clients; i++) 
		{
	  		if (FD_ISSET(sd_list[i], &rfds)) 
			{
          		recvd = recv(sd_list[i], in_message, sizeof(in_message), 0);
          		if (recvd < 0) {
	    			perror("recv");
	    			exit(1);
	  			}  /* if */
	  			if (recvd > 0) {
          				//Send processed information
						process_command(in_message, out_message, priority_list);
          				if (send(sd_list[i], out_message, sizeof(out_message), 0)<0) {
             					perror("send error");
          				}  /* if */
				memset(&in_message, 0, sizeof(in_message));
            	memset(&out_message, 0, sizeof(out_message));
	  			}  /* if */
	  			else {
					/* fix for drop-out clients */
					printf("Connection lost: FD=%d Slot=%d\n",
                                     	sd_list[i], i); 
					close(sd_list[i]);
					sd_list[i] = 0;
				} /* else */
			// break;
			} /* if */
	  } /* for */
	} /* else */

        }  /* while */

	printf("After recv.\n");

        /* close up both sockets */
	close(sd);
        
        printf("After close.\n");
        /* give client a chance to properly shutdown */
        sleep(1);
}
