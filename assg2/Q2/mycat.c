
/*
Question1: mycat
Mudasir Hanif Shaikh (ms03831) and Kainat Abbasi (ka04051)
CS 2021, Habib University
Assignment 2, OS, Fall 2019
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //for close 
#include <fcntl.h> // for open
#include <string.h>
/*
https://c-for-dummies.com/blog/?p=1758 
https://stackoverflow.com/questions/19472546/implicit-declaration-of-function-close */


int isNumbered(char* argv)
{
    if(strstr(argv, "-n") != NULL) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{ 
    int i;
    int line_count = 1;
    FILE* file;
    size_t buffer = 120;
    char *line = (char *) malloc(buffer);
    int check, check1;
    int check2 = isatty(0); //int check3 = check2 && (argc > 1);
    printf("is stdin?(1 if yes, 0 if no): %i", check2);
    if (isatty(0)) printf("%s", "yes");
    if (!check2){
        for (i = 1; i < argc; i++) {  
        check = isNumbered(argv[i]);
        check1 = isNumbered(argv[i+1]); 
        if (check1 == 0 ){
            if (check2 == 0){
                file = fopen(argv[i],"r"); //opens the file in readonly mode           
                if(file < 0) { 
                    perror("open"); 
                    return -1; 
                } 

                while ( getline ( &line, &buffer, file ) >= 0 )
                {
                    if (check) printf("%d. %s", line_count, line);
                    else printf("%s", line);
                    line_count++;
                }
            }
            else printf("%s", "stdin passed");
        fclose(file);                             
        }
        else continue;
        }
    }
    else{
        while ( getline ( &line, &buffer, stdin ) >= 0 )
            {
            printf("%s", line);
            }
    }
    
    free(line);
    return 0;
}
/*
int main(int argc,char *argv[])
{ 
    int i;
    int line_count = 1;
    int file;
    char chr;
    
    for (i = 1; i < argc; i++) {               
        printf("%d", line_count);
        file = fopen(argv[i],); //opens the file in readonly mode           
        if(file < 0) { 
            perror("open"); 
            return -1; 
        } 
        write(1, &line_count, sizeof(line_count));
        
        while(read(file, &chr, 1)){        
            write(1, &chr, 1);
            if (chr == '\n') {
                line_count++;
                printf("%i", line_count);
                write(1, &line_count, sizeof(line_count));
            }
        }
    
        close(file);                             
    }
    return 0;
}
*/