#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

typedef struct Memory_Block {
    int free;  //Stores bool to show if its free
    size_t size; //Stores the size of the memory block
    struct Memory_Block* next; //Stores the next memory block
    void* data;
} Memory_Block;

static Memory_Block *first_block;
static pthread_mutex_t mutex;
static void* memorypool;


void mem_init(size_t size){
    memorypool = malloc(size); //Initialize the memeorypool
    first_block = (Memory_Block*) malloc(sizeof(Memory_Block)); //Create the first empty block
    (*first_block).free = 1;
    (*first_block).size = size;
    (*first_block).next = NULL;
    (*first_block).data = memorypool;
}

void* mem_alloc(size_t size) {
    pthread_mutex_lock(&mutex);
    if (size == 0) {
        pthread_mutex_unlock(&mutex);
        return (*first_block).data;  //If trying to allocate 0 bytes reutn the memorypool
    }
    
    Memory_Block* current_block = first_block;
    while(current_block){ //Iterates all existing blocks in memorypool
        if ((*current_block).free == 1 && (*current_block).size >= size){
            if ((*current_block).size > size){ //If the current block is free and its size is greater than the allocating size: use it 
                Memory_Block* next_block = (Memory_Block*) malloc(sizeof(Memory_Block));//Creating new empty block
                (*next_block).free = 1;
                (*next_block).size = (*current_block).size - size;
                (*next_block).next = (*current_block).next;
                (*next_block).data = (char*)(*current_block).data + size;
                (*current_block).free = 0;
                (*current_block).size = size;
                (*current_block).next = next_block; //Setting the found block as allocated han connecting it to the new empty block
            } else {
                (*current_block).free = 0; //This will be run when current block is equal in size to the allocating
            }
            pthread_mutex_unlock(&mutex);
            return (*current_block).data; //Returns pointer to the allocated block
        }
        current_block = (*current_block).next;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void mem_free(void* block) {
    pthread_mutex_lock(&mutex);
    if(block == NULL){ //Returns if trying to free NULL
        pthread_mutex_unlock(&mutex);
        return;
    }

    Memory_Block *freed_block = NULL;
    Memory_Block *current_block = first_block;

    while(current_block){ //Iterates the list until end or if block is found
        if (block == (*current_block).data){
            freed_block = current_block; //Setting current_block as the freed one
            break;
        }
        current_block = (*current_block).next;
    }

    (*freed_block).free = 1; // Mark it as free

    int free_after = 0;
    // Merge with the next block if it's free
    Memory_Block* next_block = (*freed_block).next;
    if (next_block != NULL && (*next_block).free == 1){ //Checks if block after is free
        free_after = 1;
    }

    if (free_after == 1){
        (*freed_block).size = (*freed_block).size + (*next_block).size; //Sets the middle block to both sizes
        (*freed_block).next = (*next_block).next; //Removes the last block from pool
        free(next_block);
    
    }
    pthread_mutex_unlock(&mutex);
}

void* mem_resize(void* block, size_t size){
    mem_free(block); // Freeing the old block
    Memory_Block* new_block = mem_alloc(size); //Getting a new block with the intended size
    return new_block;
}

void mem_deinit(){
    Memory_Block *current_block = first_block;
    Memory_Block *next_block;

    free(memorypool);

    while(current_block){
        next_block = (*current_block).next;
        free(current_block);
        current_block = next_block;
    }
    first_block = NULL;
}