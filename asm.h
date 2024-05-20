#pragma once
#include "parser.h"

void declarationASM(parseTree** result);

void startCode();

void handleAsm(parseTree* t);

void identifyAsm(parseTree** result);

void handlePrint(parseTree** result);

void printNumber();

void handleWhile(parseTree** result);

void endCode();

void startCode();

void startCodeSeg();

void startDataSeg();

void conditionASM(parseTree** result, char* lastLabel);

void handleAssigment(parseTree* id);

void rewriteStr(TokenId* first, TokenId* second);

void copyFromStrToStr(TokenId* first, TokenId* second);

void copySubStr(char* str);

char* getStackLocation();

void saveToCodeSeg(char* command);

void saveToDataSeg(char* command);

void compareSignAssembly(parseTree* t, char* command);


char* handleVars(TokenId* var);


void computeExpression(parseTree** start);
