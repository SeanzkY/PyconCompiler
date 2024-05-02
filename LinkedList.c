#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"


// Function to initialize a new node
Node* createNode(void* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a new element at the beginning of the linked list
void insertAtBeginning(Node** head, void* data) {
    Node* newNode = createNode(data);
    newNode->next = *head;
    *head = newNode;
}

// Function to insert a new element at the end of the linked list
void insertAtEnd(Node** head, void* data) {
    Node* newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
    }
    else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Function to remove the first occurrence of a specified element from the linked list
void removeElement(Node** head, void* data) {
    if (*head == NULL) {
        fprintf(stderr, "List is empty\n");
        return;
    }
    Node* current = *head;
    Node* prev = NULL;
    while (current != NULL && current->data != data) {
        prev = current;
        current = current->next;
    }
    if (current == NULL) {
        fprintf(stderr, "Element not found in the list\n");
        return;
    }
    if (prev == NULL) {
        *head = current->next;
    }
    else {
        prev->next = current->next;
    }
    free(current);
}

// Function to print the elements of the linked list
void printLinkedList(Node* head, void (*printFunction)(void*)) {
    Node* current = head;
    while (current != NULL) {
        (*printFunction)(current->data);
        current = current->next;
    }
    printf("\n");
}

// Function to free the memory allocated for the linked list
void freeLinkedList(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

// Example print function for integers
void printInt(void* data) {
    printf("%d ", *((int*)data));
}

