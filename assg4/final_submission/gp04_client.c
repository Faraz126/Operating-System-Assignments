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


int RUNNING;

void * input(void * ptr)
{
    /*
    keeps taking input from the user
    */

    int sock = *((int *) ptr);
    char *buffer;
    size_t bufsize = 32;
    buffer = (char *)malloc(bufsize * sizeof(char));

    while(RUNNING)
    {
        //send the request
        if (getline(&buffer,&bufsize,stdin) != 0)
        {

            if(write(sock,buffer,strlen(buffer)) < 0) //sendint to clieng
            {
                perror("send");
            }
        }   
    }
    printf("Exiting thread\n");
    free(buffer); 
}

int main(int argc, char * argv[]){
    
    if (argc != 4)
    {
        printf("Incorrect format\n");
        return 0;
    } 
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
    if(write(sock,request,strlen(request)) < 0){ //sending name first
            perror("send");
    }
    n  = -1;
    while (n < 0) //waiting for the client to read the name 
    {
        n = read(sock, response, BUF_SIZE-1);
    }
    // read(sock, smth)
    pthread_create(&thread, NULL, input, &sock); //creating thread that takes input and sends it to server
    RUNNING = 1;

    while (RUNNING) //to keep reading from server
    {

        memset(response, 0, BUF_SIZE);
        response[0] = '\n';
        n = read(sock, response, BUF_SIZE-1);   

        if(n <= 0)
        { //closed or error on socket
                  
            printf("Client Closed With socket %d\n:", sock);
            RUNNING = 0;
            return 0;

        }
        else
        { //server sent a message
            printf("%s", response);
        }
            
        
        
    }
    
}