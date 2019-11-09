#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //for close 
#include <fcntl.h> // for open
#include <string.h>
#include <sys/mman.h>
#include <math.h>
#include <inttypes.h>
int TOTAL_SIZE = 1024;

int MAGIC = 1234567;


typedef struct __node_t 
{
    int size;
    struct __node_t* next;
} __attribute__((packed)) node_t;

typedef struct __allocated 
{
    int size;
    int magic;
} __attribute__((packed)) node;


node_t mystr;
node_t * head = &mystr;

int my_init()
{
    head = mmap(NULL, TOTAL_SIZE, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
    head-> size  = TOTAL_SIZE - sizeof(node_t);
    head-> next = NULL;
    return 1;
}

void * allocate_on_block(node_t** current, int size)
{
    node_t * temp = *current;
    char * to_move = *current;
    node * ptr_return = *current;
    to_move = to_move + size;
    *current = (node_t*) to_move; //moving the given pointer
    (*current)->next = temp->next;
    (*current)->size = temp->size - size;
    ptr_return->size = size - sizeof(node);
    ptr_return->magic = MAGIC;
    return ptr_return + 1;
}

void * my_malloc(int size)
{
    size = size + sizeof(node);
    //printf("%d\n", (int) ceil((double)size/(double)sizeof(node_t)));
    node * ptr = NULL;
    
    if (head -> size >= size)
    {
        
        /*
        ptr = temp;
        head = ptr + (int) ceil((double)size/(double)sizeof(node));
        //*head = node_t();
        head->size = temp->size - ((int) ceil((double)size/(double)sizeof(node)) * sizeof(node));
        head->next = temp->next;
        ptr->size = size - sizeof(node);
        ptr->magic = MAGIC;
        printf("%d\n", ptr->size);
        //temp->size = (ceil(size/sizeof(node_t)) * sizeof(node_t)) - sizeof(node_t);
        //temp->next = MAGIC;
        return ptr +1; //to return the start of memory
        */
       return allocate_on_block(&head, size);
    }
    else
    {
        node_t * temp = head;
        while (temp -> next != 0)
        {
            if (temp -> next -> size >= size)
            {

                /*
                ptr = temp->next; //to return
                temp->next = ptr + (int) ceil((double)size/(double)sizeof(node));
                temp->next->size = ptr->size - ((int) ceil((double)size/(double)sizeof(node)) * sizeof(node)); //as ptr points to start of temp->next currently 
                temp->next->next = ptr->magic;
                ptr->size = size - sizeof(node);
                ptr->magic = MAGIC;
                return ptr + 1; 
                //temp->next->size = tem 
                */
               return allocate_on_block(&temp->next, size);
                
            }
            temp = temp->next;
        }
    }

    return 0; //faiture
    
/*

    while (temp != NULL)
    {
        if (temp->size <= size)
        {
            void * ptr = head + head->size 
        }
    }
*/
}

void my_free(void * ptr)
{
    /*
    basically moving the of free list to where we are deleting the node.
    */
    node* temp = ptr;
    node_t * temp2 = temp - 1;
    temp2->size = temp2->size + sizeof(node) - sizeof(node_t);
    //temp2->size = ; //- (sizeof(node_t) - sizeof(node)); //since node_t is greater in size.
    temp2->next = head; //putting the head at next.
    head = temp2;
    //my_coalesce();

}

void * my_calloc(int num, int size)
{
    int total_size = num * size;
    char * my_memory = my_malloc(total_size);
    for (int i = 0; i < total_size; i++)
    {
        my_memory[i] = 0;
    }
    return my_memory;
}


int min(int num, int num2)
{
    if (num < num2)
    {
        return num;
    }
    return num2;
}
int copy(char * src_ptr, char * dest_ptr, int num_bytes)
{
    int i = 0;
    for (i = 0; i < num_bytes; i++)
    {
        dest_ptr[i] = src_ptr[i];
    }
    return i;
}


void my_coalesce()
{
    node_t * temp = head;
    char * ptr_moved_down;
    char * head_moved_down;
    while (temp->next != 0)
    {
        /* printf("HEAD %" PRIuPTR "\n", (uintptr_t)head);
        printf("TEMP %" PRIuPTR "\n", (uintptr_t)temp);
        printf("TEMP NEXT %" PRIuPTR "\n", (uintptr_t)temp->next); */
        //printf("here");
        head_moved_down = head; //doing this here since head is changing
        head_moved_down = head_moved_down + sizeof(node_t) + head->size;
        ptr_moved_down = temp->next;
        ptr_moved_down = ptr_moved_down + sizeof(node_t) + temp->next->size;
        if (ptr_moved_down == head)
        {
            /*
            for the case when any node moved down, becomes equal to head so that we can merge them together. While merging, we move the head pointer to the upper node
            */
            temp->next->size = temp->next->size + head->size + sizeof(node_t); //adding size note_t since one node is vanishing.
            temp->next->next = head->next;
            head = temp->next;
            temp->next = temp->next->next; //since the node at temp->next will become head.
        }
        //printf("%p\n", temp);
        temp = temp->next;

    }

}

void * my_realloc(void * ptr, int size)
{
    
    if (ptr == NULL)
    {
        return my_malloc(size);
    }
    else if (ptr != NULL && size == 0)
    {
        my_free(ptr);
        return NULL;
    }
    else
    {
        node* temp = ptr;
        temp--;
        int cur_size = temp->size;
        if (temp->magic == MAGIC)
        {
            int new_size = cur_size;
            void * new_ptr = my_malloc(new_size);
            int min_size = min(new_size, cur_size);
            int copied_size = copy(ptr, new_ptr, min_size);
            if (copied_size != min_size)
            {
                return NULL;
            }
            free(ptr);
            return new_ptr;
        }
    }
}

void my_showfreelist()
{
    int i = 0;
    node_t * temp = head;
    printf("\n");
    while (temp != 0)
    {
        printf(" %d: %d: %" PRIuPTR ": %" PRIuPTR "\n", i++, temp->size, (uintptr_t)temp, (uintptr_t)temp->next);
        temp = temp->next;
    }


}

void my_uninit()
{

}



int main(int argc, char *argv[])
{
    my_init();
    printf("%" PRIuPTR "\n", (uintptr_t)head);
    printf("%d\n", head->size);
    printf("%d\n", sizeof(node_t));
    printf("%d\n", sizeof(node));
    printf("%d\n%d\n",sizeof(int), sizeof(node_t*));
    void * ptr = my_malloc(100);
    node * myptr = ptr;
    printf("%d\n", myptr->size);
    printf("%" PRIuPTR "\n", (uintptr_t)ptr);
    
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
    /* ptr2 = my_calloc(4, 18);
    myptr = ptr2;
    printf("%d\n", myptr->size);
    printf("%" PRIuPTR "\n", (uintptr_t)ptr2);
    printf("%" PRIuPTR "\n", (uintptr_t)head); */
}   