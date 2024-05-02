//stack functions:

#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"
#include <string.h>


Stack* stackCreate(size_t size) // set count to 0 gets nothing
{
	Stack* s = (Stack*)malloc(sizeof(Stack));
	s->count = 0;
	s->size = size;
	s->maxStackSize = STACK_MAX_SIZE;
	s->arr = malloc(size * s->maxStackSize);
	return s;
}

int stackIsEmpty(Stack* s) { // return 1 if empty 0 if not
	return s->count == 0;
}

int stackIsFull(Stack* s) { // return 1 if full 0 if not
	return s->count == s->maxStackSize;
}

void* pop(Stack* s) { // get the pointer to the stack and pop the last number
	void* ret = malloc(s->size);
	void* dst = (char*)s->arr + --s->count * s->size;
	memcpy(ret, dst, s->size);
	return ret;

}
void* top(Stack* s) { // get stack by value and returns last number
	void* ret = malloc(s->size);
	memcpy(ret, (char*)s->arr + (s->count - 1) * s->size, s->size);
	return ret;
}
void push(Stack* s, void* num) // gets pointer to stack and another value, push it to stack returns nothing
{
	if (stackIsFull(s)) {
		s->maxStackSize *= 2;
		s->arr = realloc(s->arr, s->size * s->maxStackSize);
	}
	void* dst = (char*)s->arr + s->count++ * s->size;
	memcpy(dst, num, s->size);
}

void printStack(Stack* s) { // gets nothing and prints stack (not generic to the stack native just helper)
	if (!stackIsEmpty(s)) {
		void* val = pop(s);
		printf("%d ", *(int*)val);
		printStack(s);
		push(s, val);
	}
}

//end of stack functions