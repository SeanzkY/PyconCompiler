#pragma once


// Define a structure for a node in the linked list
typedef struct Node {
    void* data;           // Pointer to the data stored in the node
    struct Node* next;    // Pointer to the next node
} Node;


void printInt(void* data);


// Function to initialize a new node
Node* createNode(void* data);

// Function to insert a new element at the beginning of the linked list
void insertAtBeginning(Node** head, void* data);

// Function to insert a new element at the end of the linked list
void insertAtEnd(Node** head, void* data);

// Function to remove the first occurrence of a specified element from the linked list
void removeElement(Node** head, void* data);

// Function to print the elements of the linked list
void printLinkedList(Node* head, void (*printFunction)(void*));

// Function to free the memory allocated for the linked list
void freeLinkedList(Node** head);
