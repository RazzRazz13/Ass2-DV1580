#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "memory_manager.h"

typedef struct Node {
  uint16_t data; // Stores the data as an unsigned 16-bit integer
  struct Node* next; // A pointer to the next node in the List
}Node;

void list_init(Node** head, size_t size){
  mem_init(size); //Initializes the memory manager
  *head = NULL; //Sets head to null
}

void list_insert(Node** head, uint16_t data){

  Node* next_node = (Node*)mem_alloc(sizeof(Node)); //Creating a new node by allocting memory in memory manger
  (*next_node).data = data;
  (*next_node).next = NULL;
  
  Node* current_node = *head;

  if (!current_node){ //Checking if the new node is the first node
    *head = next_node; //Setting the new node to the first node
    return;
  }

  while((*current_node).next){ //Iterates the whole list
    current_node = (*current_node).next;
  }

  (*current_node).next = next_node; //Adding the new node last in the list
}

void list_insert_after(Node* prev_node, uint16_t data){
  Node* next_node = (Node*)mem_alloc(sizeof(Node)); //Creating a new node by allocting memory in memory manger

  (*next_node).data = data;
  (*next_node).next = (*prev_node).next;

  (*prev_node).next = next_node;
}

void list_insert_before(Node** head, Node* next_node, uint16_t data){
  Node* prev_node = (Node*)mem_alloc(sizeof(Node)); //Creating a new node by allocting memory in memory manger
  (*prev_node).data = data;
  (*prev_node).next = next_node;

  Node* current_node = *head;

  if (current_node == next_node){ //Checking if the first node is the node we are searching for
    *head = prev_node; //Setting the new node to the first node
    return;
  }

  while((*current_node).next != next_node){ //Iterates the list until we find the searched node
    current_node = (*current_node).next;
  }
  (*current_node).next = prev_node; //Setting the node before the searched node next value to the new node
  return;
}

void list_delete(Node** head, uint16_t data){
  Node* current_node = *head;
  Node* previous_node;

  if((*current_node).data == data){ //Checking if first node is the searched node
    *head = (*current_node).next; //Setting second node as first node
    mem_free((char*)current_node); //Freeing the space from the first node
    return;
  }

  while((*current_node).data != data){ //Iterates the list until we find the node with the searched data
    previous_node = current_node;  
    current_node = (*current_node).next;
  }

  (*previous_node).next = (*current_node).next; //Removing the node from the list
  mem_free((char*)current_node); //Freeing the memory of the node
}

Node* list_search(Node** head, uint16_t data){
  Node* current_node = *head;

  while(current_node){ //Iterates the list until we find the node with the searched data
    if ((*current_node).data == data){
      return current_node;
    }
    current_node = (*current_node).next;
  }
  return NULL;
}

void list_display(Node** head){
  Node* current_node = *head;
  printf("[");
  while(current_node){ //Iterates the list
    printf("%d",(*current_node).data); //Printing the nodes ddata
    current_node = (*current_node).next;
    if (current_node != NULL){ //Prints , if there nodes left and prints ] if its the last node
      printf(", ");
    }  else{
      printf("]");
    }
  }
}

void list_display_range(Node** head, Node* start_node, Node* end_node){
  Node* current_node = *head;
  int active = 0;
  printf("[");
  if (start_node == NULL){
    active = 1;
  }
  while(current_node){ //Iterates the list
    if (current_node == start_node){ //Sets the print to active if start_node = NULL or startnode is current node.
      active = 1;
    }
    if (active == 1){ //Prints the data if print is active
      printf("%d",(*current_node).data);
    }
    if (current_node == end_node){ //Deactivates the print if end_node is current node
      active = 0;
    }
  current_node = (*current_node).next;
  if (current_node != NULL && active == 1){ //Prints , if there is nodes left and print is active
        printf(", ");
      }
  }
  printf("]");
}

int list_count_nodes(Node** head){
  Node* current_node = *head;
  int count = 0;

  while(current_node){ //Iterates all nodes in the list
      count = count + 1; //Adds to the count
      current_node = (*current_node).next;
  }
  return count;
}

void list_cleanup(Node** head){
  *head = NULL; //Setting the head of the list to NULL
  //mem_deinit();
}