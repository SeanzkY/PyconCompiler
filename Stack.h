#pragma once
#include <stdio.h>
#include <stdlib.h>
#define STACK_MAX_SIZE 1
typedef char stack_item;
#pragma once

//The stack structure


typedef struct { // define struct
	void* arr;
	int count;
	int size;
	int maxStackSize;


}Stack;

Stack* stackCreate(size_t size); // initialzie

int stackIsEmpty(Stack* s);// return 1 if empty 0 if not

int stackIsFull(Stack* s); // return 1 if full 0 if not

void* pop(Stack* s); // pop numb

void push(Stack* s, void* num); // push num

void* top(Stack* s); // returns top

void printStack(Stack* s);