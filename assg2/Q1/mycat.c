#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  

//getline use kar





int main(int argc, char * argv[])
{
    char * option = "-n";

    int nofiles = argc - 1 ;
    int start = 1;
    int showN = 0;
    if (argc > 1 && strcmp(argv[1], option) == 0)
    {
        nofiles = argc - 2;
        start++;
        showN = 1;
    }


    if (nofiles <= 0)
    {
        
        //printf("No file to open");
    }

    int i;
    int counter = 1;
    char * newline = "\n";
    size_t bytes = 1;
    char c[1];
    char * ptr = (char*) malloc(2000);
    size_t size = 2000;
    

    if (!isatty(0))
    {
        nofiles = 1;
        while (getline(&ptr, &size, stdin) != -1)
        {
            printf("%s", ptr);
        }
        /*
        while (read(0, c, bytes))
            {

                if (counter == 1 && showN == 1)
                {
                    printf("%*d  ", 6, counter++);
                }

                printf("%s", c);
                if (strcmp(c, newline) == 0 && showN == 1)
                {
                    
                    printf("%*d  ", 6, counter++);
                }
            }
        */
    }

    else
    {
        printf("here2");
        for (i = start; i < argc; i++)
        {
            int file = open(argv[i], O_RDONLY);
            
            
            if (file < 0)
            {
                printf("Couldnt open the file\n");
            }
            else
            {
                
                while (read(file, c, bytes))
                {
                    if (counter == 1 && showN == 1)
                    {
                        printf("%*d  ", 6, counter++);
                    }

                    printf("%s", c);
                    if (strcmp(c, newline) == 0 && showN == 1)
                    {
                        
                        printf("%*d  ", 6, counter++);
                    }
                }
            }
        }
    }
    return 0;


}