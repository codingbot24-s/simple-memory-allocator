#include <stdio.h>
#include <unistd.h>



// 16 bytes for memory alignment 
typedef char ALIGN[16];
typedef union
{
    struct 
    {
        size_t size;
        unsigned iszFree;
        union header_t *next;
    };
    ALIGN stub;
    
}header_t;

header_t *head, *tail;

void* malloc (size_t size) 
{
    size_t totalSize;
    void *block;
    header_t *header;

    if (!size)
    {
        return NULL;
    }
    
}

int main () 
{
	
}

