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



char ** split(char str[], char delimter[], int * count)
{   
    char const * temp[strlen(str)];
    strcpy(temp, str);

    char const * ptr = strtok(temp, delimter);
    
    int i = 0;
    int size = 0;
    /*
    for (i = 0; i < strlen(str); i++)
    {
        
        char c = str[i];
        if (c == delimter && i + 1 != strlen(str))
        { 
            char c2 = str[i - 1];
            if (c2 != delimter)
            {
                size++;
            }
        }
        
    }
    */

    
    
    while (ptr != 0)
    {
        size++;
        ptr = strtok(NULL, delimter);

    }

    
    
    
    //size++;
    *count = size;

    char ** str_list = (char const ** )malloc(size * sizeof(char*));

    size = 0;
    ptr = strtok(str, delimter);
    while (ptr != 0)
    {
        str_list[size++] = ptr;
        ptr = strtok(NULL, delimter);

    }

    /*
    
    str_list[0] = str;
    
    size = 1;
    int string_len = strlen(str);
    for (i = 0; i < string_len; i++)
    {
        
        char c = str[i];
        if (c == delimter && i + 1 != string_len)
        { 
            char c2 = str[i - 1];
            if (c2 != delimter)
            {
                
                str[i] = '\0';
            }
            char c3 = str[i + 1];
        
            if (c3 != delimter && c3 != '\n')
            {
            
                str_list[size++] = &(str[i + 1]);
            }

        }
        
    }

    
    
    for (i = 0; i < size; i++)
    {
        printf("%s\n", str_list[i]);
    }
    */
    return str_list;
    
    

}


void run_command(char const * command, ushort wait_flag)
{
    ushort redirect = 0;
    char * filename = 0;
    int size;
    if (strstr(command, ">>"))
    {
        
        char ** command_file = split(command, REDIRECTION_APPEND, &size);
        command = command_file[0];
        //filename = command_file[1];
        filename = split(command_file[1],SPACE, &size)[0];
        redirect = 1;
    }
    else if(strstr(command, ">"))
    {
        
        char ** command_file = split(command, REDIRECTION_OVERWRITE, &size);
        command = command_file[0];
        filename = split(command_file[1],SPACE, &size)[0];
        redirect = 2;
    }

    char ** argv = split(command, SPACE, &size);
    

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
            myargs[i] = strdup(argv[i]);
        }
        myargs[size] = NULL;
        /*
        myargs[0] = strdup("wc");   // program: "wc" (word count)
        myargs[1] = strdup("p4.c"); // argument: file to count
        myargs[2] = NULL;           // marks end of array
        */
        execvp(myargs[0], myargs);  // runs word count
        exit(0);
    } else {
        // parent goes down this path (original process)
        if (wait_flag)
        {
            int pid = waitpid(rc, NULL, WUNTRACED);   
            if (pid == -1)
            {
                printf("wait failed");
            }
        }
        
    }

}



int change_directory(char const * command, char * cwd_ptr, int size_of_cwd)
{
    int size;
    char ** command_path = split(command, SPACE, &size);
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
    char * c_path = getenv("PATH");
    char new_path[2048];
    sprintf(new_path, "%s:%s", c_path, cwd);
    int status = setenv("PATH", new_path, 1);
    if (status != 0)
    {
        printf("Could not add current directory to PATH");
    }
}


int main(int argc, char * argv[])
{
    char const * prompt = "My Prompt>>>";
    size_t linesize = 500;
    char * line = (char *)malloc(linesize * sizeof(char));
    ushort run = 1;
    int command_count = 0;
    char ** list_of_commands = NULL;
    char const cwd[1024];
    getcwd(cwd, sizeof(cwd));
    add_cwd_to_path(cwd);
    
    
    while (run)
    {
        ushort wait_flag = 1;
        printf("%s", prompt);
        if (getline(&line, &linesize, stdin) == 0)
        {
            continue; //No input given to we move on to next iteration
        }
        

        if (strstr(line, "&") != NULL)
        {
            list_of_commands = split(line, AMBERSAND, &command_count);
            wait_flag = 0;
        }
        else
        {
            list_of_commands = split(line, "\n", &command_count);
            command_count = 1;
        }
        
        int i = 0;
        for (i = 0; i < command_count; i++)
        {
            if (strncmp(list_of_commands[i], "cd ", 3) == 0)
            {
                while(change_directory(list_of_commands[i], cwd, sizeof(cwd)) == -1)
                {
                    int size = sizeof(cwd);
                    char cwd[size*2];
                }
                
            }
            else if (strncmp(list_of_commands[i], "pwd", 3) == 0)
            {
                printf("%s\n", cwd);
            }

            else if (strcmp(list_of_commands[i], "mylsenv") == 0)
            {
                run_command("env", wait_flag);
            }
            else if (strstr(list_of_commands[i], "=") != NULL)
            {
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
                char ** value = getenv(sep[1]);
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
            else if (strcmp(list_of_commands[i], "exit") == 0)
            {
                printf("have to do smth");
                run = 0;
            }
            else
            {
                run_command(list_of_commands[i], wait_flag);
            }
        }
        free(list_of_commands);

        
    }

    free(line);











}