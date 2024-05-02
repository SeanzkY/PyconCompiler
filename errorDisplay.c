#include "errorDisplay.h"
#include "LinkedList.h"
#include <stdlib.h>
#include <stdio.h>
Node* circleErrors = NULL;


int isNoErrors() {
    return circleErrors == NULL;
}
// add one error to error list to write after all errors has been written
void addError(char* error) {
    Node* end;
    Node* temp;
    int add = 1;
    if (circleErrors == NULL) {
        circleErrors = createNode(error);
        circleErrors->next = circleErrors;
    }
    else {
        end = circleErrors;
        circleErrors = circleErrors->next;
        while (circleErrors != end) {
            if (strcmp(error, (char*)circleErrors->data) == 0) {
                add = 0;
            }
            circleErrors = circleErrors->next;
        }
        if (add && !strcmp(error, (char*)circleErrors->data) == 0) {
            temp = createNode(error);
            temp->next = end->next;
            end->next = temp;
            circleErrors = circleErrors->next;
        }
    }
}

// print all the errors we got
void printErrors() {
    Node* end;
    if (circleErrors == NULL) {
        return;
    }
    end = circleErrors;
    circleErrors = circleErrors->next;
    while (circleErrors != end) {
        printf("%s\n", (char*)circleErrors->data);
        circleErrors = circleErrors->next;
    }
    printf("%s\n", (char*)circleErrors->data);


}