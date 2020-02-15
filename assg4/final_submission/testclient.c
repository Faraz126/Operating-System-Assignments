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
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#define BUF_SIZE 4096

int clients[10] = {0,1,2,3,4,5,6,7,8,9};

void * input(void * ptr)
{
    /*
    keeps taking input from the user
    */

    int sock = *((int *) ptr);
    char *buffer;
    size_t bufsize = 256;
    buffer = (char *)malloc(bufsize * sizeof(char));
    strcat(buffer, "/msg ");
    int i;
    while(1)
    {

        //send the request
        for (i = 0; i < 10; i++)
        {
            strcat(buffer, "/msg ");
            char integer[64];
            sprintf(integer, "%d hello\n", i);
            strcat(buffer, integer);
            printf("%s\n", buffer);
            if(write(sock,buffer,strlen(buffer)) < 0) //sendint to clieng
            {
                perror("send");
            }
            memset(buffer, 0, 256);
            sleep(1);

        }
        
        /*
        if (getline(&buffer,&bufsize,stdin) != 0)
        {

            if(write(sock,buffer,strlen(buffer)) < 0) //sendint to clieng
            {
                perror("send");
            }
        }
        */  
    }
    free(buffer); 
}

int main(int argc, char * argv[]){
    
    assert(argc == 4);  //the hostname we are looking up
    char * hostname= argv[1];
    short port= atoi(argv[2]);                 //the port we are connecting on

    struct addrinfo *result;       //to store results
    struct addrinfo hints;         //to indicate information we want

    struct sockaddr_in *saddr_in;  //socket interent address
    pthread_t thread;
    int s,n, i;                       //for error checking

    int sock;                      //socket file descriptor

    char * request= argv[3]; //the GET request

    char response[4096];           //read in 4096 byte chunks
    memset(&hints,0,sizeof(struct addrinfo));  //zero out hints
    hints.ai_family = AF_INET; //we only want IPv4 addresses

    //Convert the hostname to an address
    if( (s = getaddrinfo(hostname, NULL, &hints, &result)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(s));
        exit(1);
    }
        //convert generic socket address to inet socket address
    saddr_in = (struct sockaddr_in *) result->ai_addr;

    //set the port in network byte order
    saddr_in->sin_port = htons(port);

    //open a socket
    if( (sock = socket(AF_INET, SOCK_STREAM, 0))  < 0){
        perror("socket");
        exit(1);
    }

    //connect to the server
    if(connect(sock, (struct sockaddr *) saddr_in, sizeof(*saddr_in)) < 0){
        perror("connect");
        exit(1);
    }
    if(write(sock,request,strlen(request)) < 0){
            perror("send");
    }
    pthread_create(&thread, NULL, input, &sock);

    fd_set activefds, readfds;
    FD_ZERO(&activefds);
    FD_SET(sock, &activefds);
    while (1)
    {
        readfds = activefds;

        memset(response, 0, BUF_SIZE);
        for(i=0; i < FD_SETSIZE; i++)
        {
            //printf("%d\n", i);
            if(FD_ISSET(i, &readfds) && i == sock)
            {
                response[0] = '\n';
                n = read(sock, response, BUF_SIZE-1);   

                if(n <= 0)
                { //closed or error on socket

                    //remove file descriptor from set
                    FD_CLR(sock, &activefds);                    
                    printf("Client Closed With socket %d\n:", sock); 
                    return 0;

                }
                else
                { //client sent a message
                    printf("%s", response);
                }
            }
        
        }
    }
    
}