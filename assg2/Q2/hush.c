#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>




char * SPACE = " \n";
char * AMBERSAND = "&\n";
char * REDIRECTION_OVERWRITE = ">\n";
char * REDIRECTION_APPEND = ">>\n";

typedef struct node Node;

typedef struct list List;


// Linklist taken from 
//https://github.com/skorks/c-linked-list/blob/master/linkedlist.c

struct node {
  int data;
  char * command_name;
  struct node * next;
};

struct list {
  Node * head; 
};


Node * createnode(int data, char * name){
  Node * newNode = malloc(sizeof(Node));
  if (!newNode) {
    return NULL;
  }
  newNode->data = data;
  newNode->command_name = (char *) malloc (strlen(name));
  strcpy(newNode->command_name, name);
  newNode->next = NULL;
  return newNode;
}

List * makelist(){
  List * list = malloc(sizeof(List));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  return list;
}

void display(List * list) {
  printf("PID \t NAME\n");
  Node * current = list->head;
  if(list->head == NULL) 
    return;
  
  for(; current != NULL; current = current->next) {
    printf("%d\t", current->data);
    printf("%s\n", current->command_name);
  }
}

void add(char * command_name, int data, List * list){
  Node * current = NULL;
  if(list->head == NULL){
    list->head = createnode(data, command_name);
  }
  else {
    current = list->head; 
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = createnode(data, command_name);
  }
}

void delete(int data, List * list){
  Node * current = list->head;            
  Node * previous = current;           
  while(current != NULL){           
    if(current->data == data){      
      previous->next = current->next;
      if(current == list->head)
        list->head = current->next;
      free(current);
    }                               
    previous = current;             
    current = current->next;        
  }                                 
}                                   


void destroy(List * list){
  Node * current = list->head;
  Node * next = current;
  while(current != NULL){
    next = current->next;
    free(current->command_name);
    free(current);
    current = next;
  }
  free(list);
}


List * PID_LIST; //linklist to store PIDs





char ** split(char * str, char delimter[], int * count)
{   
    /*
    returns the an array of pointers, each pointer pointing to a str which has been splited based on the delimter characters.
     The size of the array is stored in count.
    */

    char temp[strlen(str)];
    strcpy(temp, str);

    char * ptr = strtok(temp, delimter);
    
    int i = 0;
    int size = 0;
    
    while (ptr != 0)
    //calculating size
    {
        size++;
        ptr = strtok(NULL, delimter);

    }

    *count = size;

    char ** str_list = (char ** )malloc(size * sizeof(char *)); //new array of strings

    size = 0;
    ptr = strtok(str, delimter);
    while (ptr != 0)
    {
        str_list[size++] = ptr;
        ptr = strtok(NULL, delimter);

    }
    return str_list;
}


void run_command(char  * command, ushort wait_flag)
{
    /*
    runs the given command. 
    if wait_flag is 1, it waits for the command to finish before moving else
    if wait_flag is 0, its runs the command in background
    */
    ushort redirect = 0; //flag to keep track if we have to redirect
    char * filename = 0; //to store the filename incase of redirection
    int size; //variable to store sizes of different arrays
    if (strstr(command, ">>"))
    {
        //redirection with append
        char ** command_file = split(command, REDIRECTION_APPEND, &size);
        command = command_file[0];
        //filename = command_file[1];
        filename = split(command_file[1],SPACE, &size)[0];
        redirect = 1;
    }
    else if(strstr(command, ">"))
    {
        //redirection with write
        char  ** command_file = split(command, REDIRECTION_OVERWRITE, &size);
        command = command_file[0];
        filename = split(command_file[1],SPACE, &size)[0];
        redirect = 2;
    }

    char ** argv = split(command, SPACE, &size); //splitting the command with space
    

    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
	// child: redirect standard output to a file
        if (redirect != 0)
        {
            close(STDOUT_FILENO);
            if (redirect == 1)
                open(filename, O_CREAT | O_WRONLY | O_APPEND, S_IRWXU); 
            else
                open(filename, O_CREAT | O_WRONLY, 0777); 
        }
	

	// now exec "wc"...
        char *myargs[size + 1];
        for (int i = 0; i < size; i++)
        {
            myargs[i] = strdup(argv[i]); //preparing arguments
        }
        myargs[size] = NULL;

        execvp(myargs[0], myargs);
        printf("Command not found");
        exit(0); //incases things go off
    } else {
        // parent goes down this path (original process)
        if (wait_flag)
        { 
            int pid = waitpid(rc, NULL, WUNTRACED);//waiting for termination   
            if (pid == -1)
            {
                printf("wait failed");
            }
        }
        else
        {
            add(argv[0], rc, PID_LIST); // adding to your list of processes
        }
        
    }

}



int change_directory(char * command, char * cwd_ptr, int size_of_cwd)
{
    /*
    changes current directory based on comment,

    */
    int size;
    char  ** command_path = split(command, SPACE, &size);
    int status = chdir(command_path[1]);
    if (status < 0)
    {
        if (errno == ENOENT)
        {
            printf("Directory does not exist\n");
        }
        else
        {
            printf("Couldnt change directoy\n");
        }
    }
    else
    {
        if ((getcwd(cwd_ptr, size_of_cwd) == 0) && (errno == ERANGE))
        {
            return -1;
        }

        return 0;
    }

}



void add_cwd_to_path(char * cwd)
{

    /*
    adds . to the PATH variable.
    */
    char * c_path = getenv("PATH");
    char new_path[2048];
    sprintf(new_path, "%s:%s", c_path, "."); // appending dot at the end, so that it always point to cwd.
    int status = setenv("PATH", new_path, 1);
    if (status != 0)
    {
        printf("Could not add current directory to PATH");
    }
}


int main(int argc, char * argv[])
{
    PID_LIST = makelist(); //intializes an array
    char const * prompt = " Prompt>>>"; 
    size_t linesize = 500;
    char * line = (char *)malloc(linesize * sizeof(char)); //space to store nput
    ushort run = 1; 
    int command_count = 0; //to keep acount of number of commands to iterate over
    char ** list_of_commands = NULL;
    char cwd[1024]; //to store current wordking directory
    add_cwd_to_path(cwd);
    
    
    while (run == 1)
    {
        getcwd(cwd, sizeof(cwd)); //get current working directory.
        ushort wait_flag = 1; //by default we wait for statements to end.
        printf("%s: %s", cwd, prompt);
        if (getline(&line, &linesize, stdin) == 0 || strcmp(line, "\n") == 0)
        {
            continue; //No input given to we move on to next iteration
        }
        

        if (strstr(line, "&") != NULL)
        {
            //seperating by &
            list_of_commands = split(line, AMBERSAND, &command_count);
            wait_flag = 0;
        }
        else
        {
            //getting rid of \n if onyl one command given
            list_of_commands = split(line, "\n", &command_count);
            command_count = 1;
        }
        
        int i = 0;

        if (command_count > 1)
        {
            wait_flag = 1;
        }


        for (i = 0; i < command_count; i++)
        {
            if (strncmp(list_of_commands[i], "cd ", 3) == 0)
            {
                while(change_directory(list_of_commands[i], cwd, sizeof(cwd)) == -1)
                {
                    /*
                    increasing size till we can accomodate size
                    */
                    int size = sizeof(cwd);
                    char cwd[size*2];
                }
                
            }
            /*
            else if (strncmp(list_of_commands[i], "pwd", 3) == 0)
            {
                printf("%s\n", cwd);
            }
            */
            else if (strncmp(list_of_commands[i], "mylsenv", 7) == 0)
            {
                run_command("env", wait_flag);
            }
            else if (strstr(list_of_commands[i], "=") != NULL)
            {
                /*
                command of type var=name
                */
                int size;
                char * var;
                char * string;
                char ** sep = split(list_of_commands[i],"=",&size);
                if (size == 2)
                {
                    var = sep[0];
                    string = sep[1];
                    int status = setenv(var, string, 1);
                    if (status != 0 )
                    {
                        printf("could not edit %s\n", var);
                    }
                }
                else
                {
                    printf("syntax error\n");
                }
                free(sep);
                
            }
            else if (strncmp(list_of_commands[i], "show ", 5) == 0)
            {
                
                int size;
                char ** sep = split(list_of_commands[i],SPACE,&size);
                char * value = getenv(sep[1]);
                if (value != NULL)
                {
                    printf("%s: %s\n", sep[1], value);
                }
                else
                {
                    printf("Could not find environment variable: %s\n", sep[1]);
                }
                free(sep);
            }
            else if (strncmp(list_of_commands[i], "myps", 4) == 0)
            {
                printf("PID \t NAME\n");
                Node * current = PID_LIST->head;
                if(PID_LIST->head == NULL) 
                    continue;

                if ((current != NULL) && (waitpid(current->data, NULL, WNOHANG) != 0))
                    {
                        //getting rid of the ended processes

                        free(current->command_name);
                        delete(current->data, PID_LIST);
                    }
                
                current = PID_LIST->head;
  
                for(; current != NULL; current = current->next) {
                    printf("%d\t", current->data);
                    printf("%s\n", current->command_name);
                    if ((current->next != NULL) && waitpid(current->next->data, NULL, WNOHANG) != 0)
                    {
                        free(current->next->command_name);
                        delete(current->next->data, PID_LIST);

                    }
                }
            }
            else if (strncmp(list_of_commands[i], "exit", 4) == 0)
            {
                //exiting the process
                Node * current = PID_LIST->head;
                if(PID_LIST->head == NULL) 
                {
                    run = 0;
                    continue;
                }
                for(; current != NULL; current = current->next) {
                    //killing all processes
                    kill(current->data, SIGKILL);

                }

                run = 0;
            }
            else
            {
                run_command(list_of_commands[i], wait_flag);
            }

        }
        free(list_of_commands);

        
    }


    destroy(PID_LIST);
    free(line);











}