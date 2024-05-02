#pragma once

#include "Tokens.h"
#include <stdio.h>
#include <stdlib.h>




// Tree struct 
typedef struct parseTree {
	TokenId* val;
	struct parseTree* son;
	struct parseTree* brother;
}parseTree;

void printParseTree(parseTree* node, int depth, int lastChild);

parseTree* ParseTreeSetVal(TokenId* val);
