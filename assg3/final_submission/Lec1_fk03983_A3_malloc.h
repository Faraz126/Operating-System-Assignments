#ifndef MEMORY_H
#define MEMORY_H
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


int my_init();
void * allocate_on_block(node_t** current, int size);
void * my_malloc(int size);
void my_free(void * ptr);
void * my_calloc(int num, int size);
int min(int num, int num2);
int copy(char * src_ptr, char * dest_ptr, int num_bytes);
void my_coalesce();
void * my_realloc(void * ptr, int size);
void my_showfreelist();
void my_uninit();
#endif