#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void read_file(char ** ptr_to_ptr, FILE* fd, int* n)
{
    /*
    ptr_to_ptr: pointer to a character pointer, passing it as double pointer because getline might need to change the pointer.
    fd: file desriptor of the file to read from
    n: pointer to the the integer denoting the current count of line. Is -1 if we dont have to show the line nubmers.
    */
    size_t size = 1024;
    while(getline(ptr_to_ptr, &size, fd) != -1)
    {
        if ((*n) == -1)
        {
            printf("%s", *ptr_to_ptr);
        }
        else
        {
            printf("%*d  %s", 6, (*n)++, *ptr_to_ptr);
        }
    }
}


int main(int argc, char * argv[])
{
    char * option = "-n";
    int nofiles = argc - 1 ;
    int start = 1; //starting position of filenames
    int counter = -1;
    if (argc > 1 && strcmp(argv[1], option) == 0)
    {
        nofiles = argc - 2;
        start++;
        counter = 0; //setting it to 0 to mark that -n has been found
    }

    char * ptr = (char*) malloc(2000);
    
    if (!isatty(0)) //checking if pipe is attached
    {
        read_file(&ptr, stdin, &counter);
    }
    else if (nofiles > 0)//iterating over the given files
    {
        for (int i = start; i < argc; i++)
        {
            FILE * file = fopen(argv[i], "r");         
            if (file == NULL)
            {
                printf("Couldnt open the file\n");
            }
            else
            {
                read_file(&ptr, file, &counter);
            }
        }
    }
    else
    {
        printf("No file to open");
    }
    free(ptr);
    return 0;


}