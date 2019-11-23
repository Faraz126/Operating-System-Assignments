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

#define BUF_SIZE 4096

struct node {
  int FD;
  char * name;
  in_port_t port;
  struct node * next;
};

typedef struct node Node;

Node * head = NULL;

void add(Node * new)
{
    if (head == NULL)
    {
        head = new;
    }
    else
    {
        head -> next = new;
    }
    new->next = NULL;
}


int removeFD(int FD)
{
    Node * temp = head;
    if (temp == NULL)
    {
        return 0;
    }
    if (head -> FD == FD)
    {
        head = head->next;
        free(temp);
        return FD;
    }

    Node * temp2 = head->next;
    while (temp2 != NULL && temp2->FD != FD)
    {
        temp = temp2;
        temp2 = temp2->next;
    }

    if (temp2->FD == FD)
    {
        temp->next = temp2->next;
        free(temp2);
        return FD;
    }
    return 0;
}


void * connection(void * ptr)
{
    fd_set activefds, readfds;
    Node * client = (Node *) ptr;
    int client_socket = client->FD;
    FD_ZERO(&activefds);
    FD_SET(client_socket, &activefds);
    char response[BUF_SIZE];
    int n;
    while (1)
    {
        int i;
        readfds = activefds;
        //printf("%d LEN OF FD SET \n", FD_SETSIZE);
        for(i=0; i < FD_SETSIZE; i++){
            //printf("%d\n", i);
            if(FD_ISSET(i, &readfds) && i == client_socket)
            {
                n = read(client_socket, response, BUF_SIZE-1);   

                if(n <= 0)
                { //closed or error on socket

                        //close client sockt
                    close(client_socket);
                    removeFD(client_socket);
                    //remove file descriptor from set
                    FD_CLR(client_socket, &activefds);

                    
                    printf("Client Closed With socket %d\n:", client_socket); 
                    return 0;

                }
                else
                { //client sent a message

                    response[n] = '\0'; //NULL terminate

                    //echo messget to client
                    if (client->name == NULL)
                    {
                        Node * temp = head;
                        while (temp != NULL)
                        {
                            if (strcmp(temp->name, response) == 0)
                            {
                                close(client_socket);
                                FD_CLR(client_socket, &activefds);
                                printf("Client Closed With socket %d as its name already existis:\n", client_socket);
                                return 0;
                            }
                            temp = temp->next;
                        }
                        client->name = (char * )malloc(strlen(response));
                        strcpy(client->name, response);
                        add(client); 
                    }
                    else
                    {
                        printf("Recieved from client: %s", response);
                        if (write(client_socket,response,strlen(response)) < 0)
                        {
                            perror("send");
                        }
                    }
                    /*
                        printf("Received From: %s:%d (%d): %s",         //LOG
                            inet_ntoa(client_saddr_in.sin_addr), 
                            ntohs(client_saddr_in.sin_port), 
                            client_sock, response);
                    */
                }   
            } 
        }         
    }
    printf("Ending");
}


int main(int argc, char * argv[]){

    fd_set activefds, readfds;
    char hostname[]="127.0.0.1";   //localhost ip address to bind to
    if (argc == 1)
    {
        printf("Port number not passed");
    }
    short port=atoi(argv[1]); 
    struct sockaddr_in saddr_in;  //socket interent address of server
    struct sockaddr_in client_saddr_in;  //socket interent address of client
    pthread_t thread;

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

    if(listen(server_sock, 5)  < 0){
        perror("listen");
        exit(1);
    }

    saddr_len = sizeof(struct sockaddr_in); //length of address
    printf("Listening On: %s:%d\n", inet_ntoa(saddr_in.sin_addr), ntohs(saddr_in.sin_port));
    FD_ZERO(&activefds);
    FD_SET(server_sock, &activefds);

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

                
                Node * new_connection = (Node *) malloc(sizeof(Node));
                new_connection->FD = client_sock;
                new_connection->port = client_saddr_in.sin_port;
                new_connection->name = NULL;
                pthread_create(&thread, NULL, connection, new_connection);
                //adding to linked list
                
                //FD_SET(client_sock, &activefds);//add socket file descriptor to set

                }
                else
                {
                    printf("i am here");
                }
                /*
                else
                {
                    //otherwise client socket sent something to us
                    client_sock = i;

                    //get the address of the socket
                    //getpeername(client_sock, (struct sockaddr *) &client_saddr_in, &saddr_len);

                    //read from client and echo back
                    n = read(client_sock, response, BUF_SIZE-1);   

                    if(n <= 0){ //closed or error on socket

                        //close client sockt
                        close(client_sock);
                        removeFD(client_sock);
                        //remove file descriptor from set
                        FD_CLR(client_sock, &activefds);

                        printf("Client Closed: %s:%d (%d)\n",           //LOG
                            inet_ntoa(client_saddr_in.sin_addr), 
                            ntohs(client_saddr_in.sin_port), 
                            client_sock);

                    }
                    else
                    { //client sent a message

                        response[n] = '\0'; //NULL terminate

                        //echo messget to client
                        printf("Recieved: %s\n", response);

                        
                        printf("Received From: %s:%d (%d): %s",         //LOG
                            inet_ntoa(client_saddr_in.sin_addr), 
                            ntohs(client_saddr_in.sin_port), 
                            client_sock, response);
                        
                    }
                }
                */
            
                
            }
        }
    }

}
