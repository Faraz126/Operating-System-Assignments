#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#define BUF_SIZE 4096

int main(int argc, char * argv[]){

  fd_set activefds, readfds;
  char hostname[]="127.0.0.1";   //localhost ip address to bind to
  if (argc == 1)
  {
      printf("Port number not passed");
  }
  short port=atoi(argv[1]);               //the port we are to bind to


  struct sockaddr_in saddr_in;  //socket interent address of server
  struct sockaddr_in client_saddr_in;  //socket interent address of client

  socklen_t saddr_len = sizeof(struct sockaddr_in); //length of address

  int server_sock, client_sock;         //socket file descriptor


  char response[BUF_SIZE];           //what to send to the client
  int n;                             //length measure

  //set up the address information
  saddr_in.sin_family = AF_INET;
  inet_aton(hostname, &saddr_in.sin_addr);
  saddr_in.sin_port = htons(port);

  //open a socket
  if( (server_sock = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    exit(1);
  }

  //bind the socket
  if(bind(server_sock, (struct sockaddr *) &saddr_in, saddr_len) < 0){
    perror("bind");
    exit(1);
  }

  //ready to listen, queue up to 5 pending connectinos
  if(listen(server_sock, 5)  < 0){
    perror("listen");
    exit(1);
  }


  saddr_len = sizeof(struct sockaddr_in); //length of address

  printf("Listening On: %s:%d\n", inet_ntoa(saddr_in.sin_addr), ntohs(saddr_in.sin_port));



    FD_ZERO(&activefds);
    FD_SET(server_sock, &activefds);
  //accept incoming connections
  while(1){ //loop


    //update the set of selectable file descriptors
    readfds = activefds;

    //Perform a select
    if( select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0){
      perror("select");
      exit(1);
    }

    //check for activity on all file descriptors
    int i;
    for(i=0; i < FD_SETSIZE; i++){

      //was the file descriptor i set?
      if(FD_ISSET(i, &readfds)){

        if(i == server_sock){ //activity on server socket, incoming connection

          //accept incoming connections = NON BLOCKING
          client_sock = accept(server_sock, (struct sockaddr *) &client_saddr_in, &saddr_len);

          printf("Connection From: %s:%d (%d)\n", inet_ntoa(client_saddr_in.sin_addr), 
                 ntohs(client_saddr_in.sin_port), client_sock);

          //add socket file descriptor to set
          FD_SET(client_sock, &activefds);

        }else{

          //otherwise client socket sent something to us
          client_sock = i;

          //get the address of the socket
          getpeername(client_sock, (struct sockaddr *) &client_saddr_in, &saddr_len);

          //read from client and echo back
          n = read(client_sock, response, BUF_SIZE-1);   

          if(n <= 0){ //closed or error on socket

            //close client sockt
            close(client_sock);

            //remove file descriptor from set
            FD_CLR(client_sock, &activefds);

            printf("Client Closed: %s:%d (%d)\n",           //LOG
                   inet_ntoa(client_saddr_in.sin_addr), 
                   ntohs(client_saddr_in.sin_port), 
                   client_sock);

          }else{ //client sent a message

            response[n] = '\0'; //NULL terminate

            //echo messget to client
            write(client_sock, response, n);

            printf("Received From: %s:%d (%d): %s",         //LOG
                   inet_ntoa(client_saddr_in.sin_addr), 
                   ntohs(client_saddr_in.sin_port), 
                   client_sock, response);
          }

        }

      }

    }

  }

  return 0; //success
}