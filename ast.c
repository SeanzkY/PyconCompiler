#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include <string.h>
#include "hash.h"
#include "ast.h"
#include "errorDisplay.h"



int isExpression(TokenId* t) {
	if (t->id == NULL)
		return 0;
	return t->tok == 1 && strcmp(t->id, "expression") == 0;
}

int getPrecedence(parseTree* t) {
	TokenId* res = t->son->brother->val;
	return res->precedence;
}

int operatorExists(parseTree* t) {
	return t->son->brother != NULL;
}

void changePrecedence(parseTree** t) {
	parseTree* firstExp = *t; 
	if (firstExp) {
		if (!isExpression((*t)->val) ) {
			changePrecedence(&(firstExp->brother));
			changePrecedence(&(firstExp->son));
			return;
		}
		else {
			changePrecedence(&(firstExp->brother));
		}
	}
	else
		return;
	firstExp = firstExp->son;
	Node* postFix = createNode(firstExp->val);
	Node* start = postFix;
	Stack* operators = stackCreate(sizeof(TokenId));
	Stack* subTrees = stackCreate(sizeof(parseTree));
	parseTree* precenceTree = NULL;
	TokenId* temp;
	parseTree* tempTree;
	firstExp = firstExp->brother;
	while (firstExp) {
		if (firstExp->val->tok == OPERATOR || firstExp->val->tok == LOGIC_OPERATORS) {
			if (stackIsEmpty(operators) || ((TokenId*)top(operators))->precedence < firstExp->val->precedence) {
				push(operators, firstExp->val);
			}
			else {
				while (!stackIsEmpty(operators) && ((TokenId*)top(operators))->precedence >= firstExp->val->precedence) {
					
					postFix->next = createNode((TokenId*)pop(operators)); 
					postFix = postFix->next;
				}
				push(operators, firstExp->val);
			}
			firstExp = firstExp->brother->son;
		}
		else {
			postFix->next = createNode(firstExp->val);
			postFix = postFix->next;
			firstExp = firstExp->brother;
		}
		
	}
	while (!stackIsEmpty(operators)) {

		postFix->next = createNode(pop(operators));
		postFix = postFix->next;
	}
	while (start) {
		if (((TokenId*)start->data)->tok == OPERATOR) {
			precenceTree = ParseTreeSetVal((createTokenId(-1,"expression",-1)));
			precenceTree->son = (parseTree*)pop(subTrees);
			precenceTree->son->brother = ParseTreeSetVal(((TokenId*)start->data));
			if (((parseTree*)top(subTrees))->val->tok != -1) {
				precenceTree->son->brother->brother = ParseTreeSetVal((createTokenId(-1, "expression", -1)));
				precenceTree->son->brother->brother->son = (parseTree*)pop(subTrees);
				if (precenceTree->son->val->tok != -1) {
					temp = precenceTree->son->brother->brother->son->val;
					precenceTree->son->brother->brother->son->val = precenceTree->son->val;
					precenceTree->son->val = temp;
				}
				else {
					tempTree = precenceTree->son;
					precenceTree->son = precenceTree->son->brother->brother;
					precenceTree->son->brother = tempTree->brother;
					tempTree->brother = NULL;
					precenceTree->son->brother->brother = tempTree;
					if (precenceTree->son->brother->brother->val->tok != -1) {
						tempTree = precenceTree->son->brother->brother;
						precenceTree->son->brother->brother = ParseTreeSetVal((createTokenId(-1, "expression", -1)));
						precenceTree->son->brother->brother->son = tempTree;

					}
				}
				
			}
			else {
				precenceTree->son->brother->brother = (parseTree*)pop(subTrees);
				tempTree = precenceTree->son;
				precenceTree->son = precenceTree->son->brother->brother;
				precenceTree->son->brother = tempTree->brother;
				tempTree->brother = NULL;
				precenceTree->son->brother->brother = tempTree;
				if (precenceTree->son->brother->brother->val->tok != -1) {
					tempTree = precenceTree->son->brother->brother;
					precenceTree->son->brother->brother =  ParseTreeSetVal((createTokenId(-1, "expression", -1)));
					precenceTree->son->brother->brother->son = tempTree;

				}

			}
			

			
			
			push(subTrees, precenceTree);
			
		}
		else {
			push(subTrees, ParseTreeSetVal((TokenId*)start->data));
		}
		printf("%s", ((TokenId*)start->data)->id);
		start = start->next;
	}
	
	if (((parseTree*)top(subTrees))->val->tok != -1) {
		precenceTree = ParseTreeSetVal((createTokenId(-1, "expression", -1)));
		precenceTree->son = pop(subTrees);
	}
	else
		precenceTree = pop(subTrees);
	precenceTree->brother = (*t)->brother;
	(*t) = precenceTree;
	printf("\n");
	//*t = precenceTree;
}

void checkExpressionsVars(parseTree* t) {
	char* temp;
	int size_needed;
	if (t) {
		if (t->son && isExpression(t->val) && t->son && t->son->val->tok == IDENTIFIER && 
			symbolGet(t->son->val->id, t->son->val->scope) && symbolGet(t->son->val->id, t->son->val->scope)->type != TYPE_INTEGER_DECLARE) {
			 size_needed = snprintf(NULL, 0,"unsuitable type of var used error in line %d : %s  ", t->son->val->line, t->son->val->id);
			 temp = (char*)malloc(size_needed+1);
			sprintf(temp, "unsuitable type of var used error in line %d : %s", t->son->val->line, t->son->val->id);
			addError(temp);
		}
		checkExpressionsVars(t->son);
		checkExpressionsVars(t->brother);
	}
}
void checkDeclaration(parseTree* t) {
	char* temp;
	int size_needed;
	if (t) {
		if (t->son && t->val->tok == 1) {
			if (strcmp(t->val->id, "declaration") == 0
				&& t->son->val->tok == TYPE_STRING_DECLARE && t->son->brother->brother->brother &&
				t->son->brother->brother->brother->val->tok == IDENTIFIER &&
				symbolGet(t->son->brother->brother->brother->val->id, t->son->brother->brother->brother->val->scope) != NULL &&
				symbolGet(t->son->brother->brother->brother->val->id, t->son->brother->brother->brother->val->scope)->type != TYPE_STRING_DECLARE) {
				size_needed = snprintf(NULL, 0, "unsuitable type of var used error in line %d : %s  ", t->son->val->line, t->son->val->id);
				temp = (char*)malloc(size_needed+1);
				sprintf(temp, "unsuitable type of var used error in line %d : %s", t->son->val->line, t->son->brother->brother->brother->val->id);
				addError(temp);
			}
			else if (strcmp(t->val->id, "assignment") == 0 && t->son->val->tok == IDENTIFIER &&
				symbolGet(t->son->val->id, t->son->val->scope)
				&& symbolGet(t->son->val->id, t->son->val->scope)->type == TYPE_INTEGER_DECLARE 
				&& (t->son->brother->brother->val->tok == STRING_LITERAL || 
					(t->son->brother->brother->val->tok == IDENTIFIER
					&& symbolGet(t->son->brother->brother->val->id, t->son->brother->brother->val->scope)
					&& symbolGet(t->son->brother->brother->val->id, t->son->brother->brother->val->scope)->type == TYPE_STRING_DECLARE))) {
				size_needed = snprintf(NULL, 0, "unsuitable type of var used error in line %d : %s  ", t->son->val->line, t->son->val->id);
				temp = (char*)malloc(size_needed+1);
				sprintf(temp, "unsuitable type of var with wrong value used error in line %d : %s", t->son->val->line, t->son->brother->brother->val->id);
				addError(temp);
			}
			else if (strcmp(t->val->id, "assignment") == 0 && t->son->val->tok == IDENTIFIER &&
				symbolGet(t->son->val->id, t->son->val->scope) && symbolGet(t->son->val->id, t->son->val->scope)->type == TYPE_STRING_DECLARE
				&& t->son->brother->brother->val->id && t->son->brother->brother->val->tok == 1 &&
				strcmp(t->son->brother->brother->val->id, "expression") == 0) {
				size_needed = snprintf(NULL, 0, "unsuitable type of var used error in line %d : %s  ", t->son->val->line, t->son->val->id);
				temp = (char*)malloc(size_needed+1);
				sprintf(temp, "unsuitable type of var with wrong value used error in line %d : %s", t->son->val->line, t->son->brother->brother->val->id);
				addError(temp);
			}
		}
		

		checkDeclaration(t->son);
		checkDeclaration(t->brother);
	}

}
void checkAssignments(parseTree* t) {
	parseTree* helper;
	if (t->val->tok == IDENTIFIER && symbolGet(t->val->id, t->val->scope)->type != INTEGER_LITERAL) {
		helper = t->brother->brother;
		if (symbolGet(helper->val->id, helper->val->scope)->type == INTEGER_LITERAL) {
			printf("SPECIAL error in line %d \n", t->val->line);
		}
	}
}




void removeParenthesis(parseTree* t,int precedence) {
	TokenId* ret = (TokenId*)malloc(sizeof(TokenId));
	ret->id = _strdup("expression");
	parseTree* temp = NULL;
	parseTree* helper;
	if (t && t->val->tok != END_OF_FILE) {
		if (strcmp(t->val->id, "condition") == 0) {
			printf("");
		}
		if (isExpression(t->val)) {
			if (t->son->val->tok == OPEN_PARENTHESES) {
				removeParenthesis(t->son->brother,precedence + 10);
				removeParenthesis(t->son->brother->brother, precedence+ 10);
				// this line is for non expression stuff
				removeParenthesis(t->brother, precedence);
				if (t->son->brother->brother) {
					temp = t->son->brother->brother->brother;
				}
				t->son = t->son->brother->son;
				if (temp) {
					helper = temp->brother;
					temp->brother  = ParseTreeSetVal(ret);
					temp->brother->son = helper;
					if (temp->brother->son->brother) {
						helper = temp->brother->son->brother;
						temp->brother->son->brother = temp->brother->son->brother->son;
						helper->son = helper->son->brother;
						helper->son->brother = helper->son->brother;
						temp->brother->son->brother->brother = helper;
						t->son->brother->brother->son->brother = temp;
					}
					else if (t->son->brother->brother->son) {
						helper = t->son->brother->brother->son;
						while (helper->brother) {
							helper = helper->brother->brother->son;
						}
						helper->brother = temp;
						temp->brother = temp->brother->son;
					}
				}
			}
			
			else {
				if (t->son->brother) {
					t->son->brother->val->precedence += precedence;
					removeParenthesis(t->son->brother->brother, precedence);
					removeParenthesis(t->brother, precedence);
				}
				

			}
		}
		else if (t->val->tok == CLOSED_PARENTHESESE) {
			removeParenthesis(t->brother, precedence - 10);
			if(t->brother)
				removeParenthesis(t->brother->brother, precedence - 10);
		}
		else {
			if (t->val->tok == OPERATOR && t->val->precedence < 10)
				t->val->precedence += precedence;
			removeParenthesis(t->son, precedence);
			removeParenthesis(t->brother, precedence);
		}
	
		
	}
	
}

void checkParamatersType(parseTree* t) {	
	if (t) {
		checkExpressionsVars(t);
		checkAssignments(t);
		checkParamatersType(t->brother);
		checkParamatersType(t->son);
	}
	
}

