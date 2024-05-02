#pragma once

#include "parser.h"
#include "hash.h"
#define astTree parseTree


int isExpression(TokenId* t) ;

int getPrecedence(parseTree* t);
void fixDups(parseTree* t);
void checkDeclaration(parseTree* t);

astTree* changePrecedence(parseTree** t);


void checkExpressionsVars(parseTree* t);


void checkAssignments(parseTree* t);


void removeParenthesis(parseTree* t, int precedence);

astTree* removeUnneccesary(parseTree* t);

void checkParamatersType(parseTree* t);


astTree* toAst(parseTree* t);