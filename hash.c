#define _CRT_SECURE_NO_WARNINGS
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tokens.h"
#include "errorDisplay.h"


void cpyTable(hashTable* save) {
    int i;
    for (i = 0; i < TABLESIZE; i++) {
        (*save)[i] = symbolTable[i];
    }
}

void restoreHashTable(hashTable save) {
    int i;
    for (i = 0; i < TABLESIZE; i++) {
        symbolTable[i] = save[i];
    }
}

void saveUntilChr(char** word, char letter) {
    char* tempAlloc = NULL;
    if (!word)
        return;
    char* end = strrchr(*word, letter);
    if (!end ) {
        printf("major error\n");
        return;
    }
        
    int size = (end - *word);
    if (size <= 0)
        return;
    tempAlloc = (char*)realloc(*word, size + 2);
    if (tempAlloc)
        *word = tempAlloc;
    else
        return;
    (*word)[size] = '\0';
  
}

int hash(const char* key,const char* keyAdd) {
   
    char* helper = (char*)malloc(strlen(key) * sizeof(char) + strlen(keyAdd) * sizeof(char) + 1);
    if (!helper)
        return NULL;
    strcpy(helper, key);
    strcpy(helper + strlen(key), keyAdd);
    unsigned long hashResult = 5381;
    int c;
    key = helper;
    while ((c = *key++)) {
        hashResult = ((hashResult << 5) + hashResult) + c; /* hash * 33 + c */
    }
    free(helper);
    return hashResult % TABLESIZE;
}

void tableInsert(IdentiferType* val) {
    int pos = hash(val->name, val->scope);
    Node* temp;
    if (symbolTable[pos] == NULL) {
        symbolTable[pos] = createNode(val);
    }
    else {
        temp = symbolTable[pos];
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = createNode(val);
    }
}

void tableDelete(TokenId* val) {
    int pos = hash(val->id, val->scope);
    Node* temp;
    temp = symbolTable[pos];
    if (symbolTable[pos] == NULL)
        return;
    if (strcmp(((IdentiferType*)(temp->data))->name, val->id) != 0 ||
        strcmp(((IdentiferType*)(temp->data))->scope, val->scope) != 0) {

        while (temp->next != NULL && (strcmp(((IdentiferType*)(temp->next->data))->name, val->id) != 0 ||
            strcmp(((IdentiferType*)(temp->next->data))->scope, val->scope) != 0)) {
            temp = temp->next;
        }
        if (temp->next == NULL)
            return;
        temp->next = temp->next->next;
    }
    else {
        symbolTable[pos] = temp->next;
    }
  
}

int isSubString(char* word, char* subWord) {
    return strstr(word, subWord) != NULL;
}

IdentiferType* symbolGet(const char* key, char* keyAdd) {
    char* help;
    int x,y;
    IdentiferType* result;
    int pos = hash(key,keyAdd);
    Node* temp = symbolTable[pos];
    while ( temp != NULL &&  (strcmp(((IdentiferType*)temp->data)->name,key) != 0 || !isSubString(keyAdd,((IdentiferType*)temp->data)->scope) != 0) )
    {
        temp = temp->next;
    }
    if (temp == NULL && strlen(keyAdd) == 1)
        return temp;
    else if (temp == NULL && strlen(keyAdd) != 1) {
        help = _strdup(keyAdd);
        saveUntilChr(&help, '.');
        result =  symbolGet(key, help);
        free(help);
        return result;

    }
    if (temp)
        return temp->data;
    else
        return NULL;
   
}

char* findVarScope(const char* key) {
    int i;
    Node* temp;
    for (i = 0; i < TABLESIZE; i++) {
        temp = symbolTable[i];
        while (temp != NULL &&  strcmp(key, ((IdentiferType*)temp->data)->name) != 0) {
            temp = temp->next;
        }
        if (temp)
            return ((IdentiferType*)temp->data)->scope;
    }
    return NULL;
}

void tableUpdate(const char* key, char* keyAdd, IdentiferType* val) {
    *symbolGet(key, keyAdd) = *val;
}


int declareIdentifier(TokenId* identifier,Token currType) {
    char* message = "an error has occurred on line: %d, identifer: %s is already defined in scope\n";
    char* helper = malloc(strlen(message) + strlen(identifier->id) + 10);
    IdentiferType* temp = symbolGet(identifier->id, identifier->scope);
    if (temp == NULL) {
        temp = createEmptyIdentifer(identifier->id, identifier->scope, identifier->line, identifier->scopeIndex);
        temp->isSet = 1;
        temp->type = currType;
        tableInsert(temp);
    }
    else if (temp->isSet == 1) {
        sprintf(helper, message, identifier->line, identifier->id);
        addError(helper);
        return 0;
    }
    else {
        if (!temp->isFunction) {
            temp->isSet = 1;
            temp->type = currType;
        }
        return 1;
    }
}

int isDeclaredIdentifier(TokenId* identifier) {
    char* message = "an error has occurred on line: %d, identifer: %s is not defined in scope\n";
    char* helper = malloc(strlen(message) + strlen(identifier->id) + 10);
    IdentiferType* temp = symbolGet(identifier->id, identifier->scope);
    if (temp == NULL || (temp != NULL && temp->isSet == 0)) {
        sprintf(helper, message, identifier->line, identifier->id);
        addError(helper);

    }
    return temp != NULL && temp->isSet == 1;
}




