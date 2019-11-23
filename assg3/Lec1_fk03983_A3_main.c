#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "Lec1_st03983_A3_malloc.h"



int main(int argc, char *argv[])
{
    
    my_init();
    /*
    printf("%" PRIuPTR "\n", (uintptr_t)head);
    printf("%d\n", head->size);
    printf("%d\n", sizeof(node_t));
    printf("%d\n", sizeof(node));
    */
    //printf("%d\n%d\n",sizeof(int), sizeof(node_t*));
    void * ptr = my_malloc(1024);
    void * ptr1 = my_malloc(100);
    void * ptr2 = my_malloc(100);
    my_showfreelist();
    my_free(ptr2);
    my_showfreelist();
    my_realloc(ptr1, 100);
    my_showfreelist();
    my_free(ptr1);
    //node * myptr = (node * ) ptr - 1;
    //printf("%d\n", myptr->size);
    //printf("%" PRIuPTR "\n", (uintptr_t)ptr);
    /*
    printf("%" PRIuPTR "\n", (uintptr_t)head);
    printf("%d\n", head->size);
    void * ptr1 = my_malloc(100);
    myptr = ptr1;
    printf("%d\n", myptr->size);

    printf("%" PRIuPTR "\n", (uintptr_t)ptr1);
    printf("%" PRIuPTR "\n", (uintptr_t)head);
    printf("%d\n", head->size);
    void * ptr2 = my_malloc(100);
    myptr = ptr2;
    printf("%d\n", myptr->size);
    printf("%" PRIuPTR "\n", (uintptr_t)ptr2);
    printf("%" PRIuPTR "\n", (uintptr_t)head);
    printf("%d\n", head->size);
    printf("Showing FREELIST \n");
    my_showfreelist();
    my_free(ptr1);
    printf("Showing FREELIST \n");
    my_showfreelist();
    my_free(ptr2);
    printf("Showing FREELIST \n");
    my_showfreelist();
    ptr2 = my_calloc(4, 18);
    printf("Showing FREELIST \n");
    my_showfreelist();
    myptr = ptr2;
    printf("%" PRIuPTR "\n", (uintptr_t)ptr2);
    printf("%" PRIuPTR "\n", (uintptr_t)head); 
    ptr2 = my_realloc(ptr2, 500);
    printf("Showing FREELIST \n");
    my_showfreelist();
    */

}   
