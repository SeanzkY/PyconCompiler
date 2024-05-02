#pragma once

#include "Identifier.h"
#include "LinkedList.h"
#include "Tokens.h"

#define TABLESIZE 100

// this node is type identiferType* !
typedef Node *hashTable[TABLESIZE];


hashTable symbolTable;

// copies table from orginal to save
void cpyTable(hashTable* save);

// copies table from save to orginal
void restoreHashTable(hashTable save);

// hashing
int hash(const char* key, const char* keyAdd);

// insert
void tableInsert(IdentiferType* val);

// delete
void tableDelete(TokenId* val);

// update
void tableUpdate(const char* key, char* keyAdd,IdentiferType* val);

// delete .number from number (string)
void saveUntilChr(char** word, char letter);

// this function declares an identifier and check if it's legal and return 0 if there was an error
int declareIdentifier(TokenId* identifier,Token currType);

// get identifier from table
IdentiferType* symbolGet(const char* key, char* keyAdd);

char* findVarScope(const char* key);

// return if identifier is already declared or not
int isDeclaredIdentifier(TokenId* identifier);



