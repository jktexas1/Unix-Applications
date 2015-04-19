#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv)
{
  char 	input_file[100]; // Input file name

  char  in_message[100];  // Message received from the server
  
  char  hostname[100];  // Stores the name of server hostname
  int   port;           // Variable to store port number
  bool  debug_message;  // Determines whether to print debug messages
  char  user_name[100]; // Holds user's name

  /* Socket related variables */
  
  int   sd;
  struct sockaddr_in sin;
  struct sockaddr_in pin;
  struct hostent *hp;

  int j;

  /*********************************************/
  /* Step 0: Read input command line arguments */
  /*********************************************/
  printf("%s\n","Note to user: Subtraction of a quantity greater than current quantity will result in a subtraction of zero.");


  if (argc == 5) { // No input file

     strcpy(hostname,argv[1]);

     port = atoi(argv[2]);

     if      (strcmp(argv[3],"yes") == 0) debug_message=1;
     else if (strcmp(argv[3],"no") == 0)  debug_message=0;
     else { printf("Debug message flag invalid\n"); return -2; }

	 strcpy(user_name,argv[4]);

	 for(j=0;j<15;j++)
	 {
		 if(!isalpha(user_name[j]))
		 {
			 user_name[j] = 'Z';
		 }
	 }

     // The following print statement is just to confirm input
     if (debug_message)
        printf("Input - command line: %s %s %d %d\n",
                argv[0],hostname,port,debug_message);
   
  } else if (argc == 6) { // Input file provided
     
     strcpy(hostname,argv[1]);

     strcpy(input_file,argv[5]);

     port = atoi(argv[2]);

     if      (strcmp(argv[3],"yes") == 0)  debug_message=1;
     else if (strcmp(argv[3],"no") == 0)   debug_message=0;
     else { printf("Debug message flag invalid\n"); return -2; }

	 strcpy(user_name,argv[4]);

	 for(j=0;j<15;j++)
	 {
		 if(!isalpha(user_name[j]))
		 {
			 user_name[j] = 'Z';
		 }
	 }

     // The following print statement is just to confirm input
     if (debug_message)
	 printf("Input - text file: %s %s %d %d %s\n",
                 argv[0],hostname,port,debug_message,input_file);
	
     } else { // This is in case we incorrectly specify input arguments
         printf("Invalid command line arguments: %s [hostname] [port] [debug] [user] [input file]\n",argv[0]); 
         return -1;
     }

   /***************************************************/
   /* Step 1 : Create socket and establish connection */
   /***************************************************/

   /* First, find out about the desired host machine */
   if ((hp = gethostbyname(hostname)) == 0) {
        perror("gethostbyname");
        exit(1);
   }

   if (debug_message)
        printf("[Client] After gethostbyname.\n");

   /* Then, fill in the socket structure with host information */
   memset(&pin, 0, sizeof(pin));
   pin.sin_family = AF_INET;
   pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
   pin.sin_port = htons(port);

   if (debug_message)
       printf("[Client] After fill in the socket struct.\n");

   /* Grab an Internet domain socket */
   if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
   }

   if (debug_message)
       printf("[Client] After socket.\n");

    /* connect to PORT on HOST */
    if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
        perror("connect");
	exit(1);
    }

    if (debug_message)
        printf("[Client] After connect.\n");

    /*****************************************************/
    /* Step 2: Process input and communicate with server */
    /*****************************************************/

    /* Step 2.1: Input file was specified */
    if ( argc == 6 ) {

       FILE *in_file;
       char line[15];
	   char buffer[100];

       // Open user specified filename from step 0
       in_file = fopen(input_file,"r");

       if (!input_file) {
           printf("\nFile: %s was not found\n",input_file);
	   return -1; }

       // Read file text line by line
       while(fgets(line,15,in_file)) {
		  bzero(buffer,100);
		  snprintf(buffer,sizeof buffer,"%s %s",user_name,line);

          /* Send input line to the server PORT on machine HOST */
          if (send(sd, buffer, sizeof(buffer), 0) == -1) {
			perror("send");
			exit(1); }

          if (debug_message)
              printf("[Client] Sent message: %s",line);

          /* Wait for a message to come back from the server */
          if (recv(sd, in_message, sizeof(in_message), 0) == -1) {
                  perror("recv");
                  exit(1);
          }

          /* We define an input END command to specify the end of the file.          */
          /* While we send "END" message to the server, we do not want anything back */
          if (strcmp(line,"END")!=0) {
              
              /* Print the message we have received from the server */
              printf("[Client] Received message: %s.\n",in_message);
          }
        }

     /* Step 2.2: Standard input shall be used instead */
     } else if ( argc == 5 ) { 
	   char line[100];
	   char buffer[100];

       printf("Enter commands (END - to quit):\n");
		
       while (true) {

          printf("[Input] ");

         /* Read line from standard input */
	     bzero(line,100);
		 fgets(line,99,stdin);
		 bzero(buffer,100);
		 snprintf(buffer,sizeof buffer,"%s %s\n",user_name,line);
         /* Send the command to the server */
         if (send(sd,buffer,sizeof(buffer),0)== -1) {
             perror("send");
             exit(1);
         }

         if (debug_message)
             printf("[Client] Sent message: %s",buffer);

         if (strcmp(line,"END\n") != 0) { 

             /* Receive the response from the server */
			 bzero(in_message, 100);
             if (recv(sd,in_message,sizeof(in_message),0) == -1) {
	         perror("recv");
	         exit(1);
	     }

	     printf("[Client] Received message: %s.\n",in_message);
	  } else break;
	}

      }

      /* Step 3: Close the connection */
      close(sd);

      if (debug_message)
          printf("[Client] After close\n");

      return 0;
}
