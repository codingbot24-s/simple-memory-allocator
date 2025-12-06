#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// 16 bytes for memory alignment
typedef char ALIGN[16];
typedef union
{
    struct
    {
        size_t size;
        unsigned isFree;
        union header_t *next;
    } s;
    ALIGN stub;

} header_t;

header_t *head, *tail;

// lock
pthread_mutex_t global_malloc_lock;

header_t *get_free_block(size_t size)
{
    header_t *curr = head;
    while (curr)
    {
        if (curr->s.isFree && curr->s.size >= size)
        {
            return curr;
        }
        curr = curr->s.next;
    }

    return NULL;
}

void *malloc(size_t size)
{
    size_t totalSize;
    void *block;
    header_t *header;

    if (!size)
    {
        return NULL;
    }

    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if (header)
    {
        header->s.isFree = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        // we need to hide the header info from user so jumping one byte
        return (void *)(header + 1);
    }
    totalSize = sizeof(header_t) + size;
    block = sbrk(totalSize);
    if (block == (void *)-1)
    {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    header = block;
    header->s.size = size;
    header->s.isFree = 0;
    header->s.next = NULL;

    if (!head)
    {
        head = header;
    }

    if (tail)
    {
        tail->s.next = header;
    }

    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void *)(header + 1);
}

void free(void *block)
{
    header_t *header, *temp;
    void *programBreak;
    if (!block)
    {
        return NULL;
    }

    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t *)block - 1;

    programBreak = sbrk(0);

    // check if the block at the end of the heap
    if ((char *)block + header->s.size == programBreak)
    {
        if (head == tail)
        {
            head = tail = NULL;
        }
        else
        {
            temp = head;
            while (temp)
            {
                if (temp->s.next == tail)
                {
                    temp->s.next == NULL;
                    tail = temp;
                }
                temp = temp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    // not the end of heap so free but not released
    header->s.isFree = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

int main()
{
}
