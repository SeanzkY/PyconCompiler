#pragma once

#include "parser.h"
#include "hash.h"
#define astTree parseTree


int isExpression(TokenId* t) ;

int getPrecedence(parseTree* t);

void checkDeclaration(parseTree* t);



void checkExpressionsVars(parseTree* t);


void checkAssignments(parseTree* t);


void removeParenthesis(parseTree* t, int precedence);


void checkParamatersType(parseTree* t);


void changePrecedence(parseTree** t);