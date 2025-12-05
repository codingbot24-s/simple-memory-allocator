#include <stdio.h>
#include <unistd.h>


void* malloc (size_t size) 
{
    
    void* block;
    block =  sbrk(size);
    if (block == (void*) -1) {
        return NULL;   
    }
	return block;
}


typedef struct 
{
    size_t size;
    unsigned is_free;
    header_t *next;

}header_t;


int main () {
	
}

