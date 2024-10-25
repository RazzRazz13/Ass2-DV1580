#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

typedef struct Memory_Block {
    int free;  //Stores bool to show if its free
    size_t size; //Stores the size of the memory block
    struct Memory_Block* next; //Stores the next memory block
    void* data; //Stores where the Blocks data are
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

void* mem_alloc_unlocked(size_t size) {
    if (size == 0) {
        return memorypool;  //If trying to allocate 0 bytes reutn the memorypool
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
            return (*current_block).data; //Returns pointer to the allocated block
        }
        current_block = (*current_block).next;
    }
    return NULL;
}

void mem_free_unlocked(void* block) {

    if(block == NULL){ //Returns if trying to free NULL
        return;
    }

    Memory_Block *freed_block = NULL;
    Memory_Block *prev_block = NULL;
    Memory_Block *current_block = first_block;

    while(current_block){ //Iterates the list until end or if block is found
        if (block == (*current_block).data){
            freed_block = current_block; //Setting current_block as the freed one
            break;
        }
        prev_block = current_block;
        current_block = (*current_block).next;
    }
    if (freed_block == NULL) {
        return; //If freed block isnt found return
    }

    (*freed_block).free = 1; // Mark it as free

    int free_after = 0;
    int free_before = 0;

    Memory_Block* next_block = (*freed_block).next;
    if (next_block != NULL && (*next_block).free == 1){ //Checks if block after is free
        free_after = 1;
    }

    if (prev_block != NULL && (*prev_block).free == 1){ //Checks if block after is free
        free_before = 1;
    }

    // Merge with the next and previous block if it's free
    if (free_after == 1 && free_before == 1){
        (*prev_block).size = (*freed_block).size + (*next_block).size + (*prev_block).size; //Sets the middle block to all sizes
        (*prev_block).next = (*next_block).next; //Removes the last and middle block from pool
        free(next_block); //Frees the last block
        free(freed_block); //Frees the middle block

    }else if(free_after == 1){
        (*freed_block).size = (*freed_block).size + (*next_block).size; //Sets the middle block to both sizes
        (*freed_block).next = (*next_block).next; //Removes the last block from pool
        free(next_block); //Frees the last block
    }else if(free_before == 1){
        (*prev_block).size = (*freed_block).size + (*prev_block).size; //Sets the first block to both sizes
        (*prev_block).next = (*freed_block).next; //Removes the middle block from the pool
        free(freed_block); //Frees the middle block
    }
}

void* mem_alloc(size_t size){
    pthread_mutex_lock(&mutex); //Locking the mutex
    void* new_block = mem_alloc_unlocked(size); //Calling the real mem_alloc
    pthread_mutex_unlock(&mutex);//Unlocking the mutex
    return new_block;
}

void mem_free(void* block){
    pthread_mutex_lock(&mutex);//Locking the mutex
    mem_free_unlocked(block); //Calling the real mem_free
    pthread_mutex_unlock(&mutex);//Unlocking the mutex
}

void* mem_resize(void* block, size_t size){
    pthread_mutex_lock(&mutex); //Locking the mutex
    mem_free_unlocked(block); // Freeing the old block
    Memory_Block* new_block = mem_alloc_unlocked(size); //Getting a new block with the intended size
    pthread_mutex_unlock(&mutex);//Unlocking the mutex
    return new_block;
}

void mem_deinit(){
    Memory_Block *current_block = first_block;
    Memory_Block *next_block;

    free(memorypool); //Frees the memorypool for all the data.
    first_block = NULL;

    while(current_block){ //Iterates the whole list
        next_block = (*current_block).next;
        free(current_block); //Frees the block from the memory
        current_block = next_block;
    }
}