#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "FileAbstraction.h"
#include <string.h>
#include "Automate.h"
#include "Tokens.h"
#include "Stack.h"
#include "LinkedList.h"
#include "parserAutomation.h"
#include "parser.h"
#include "hash.h"
#include "ast.h"
#include "asm.h"
#include "errorDisplay.h"


void printTokens(TokenId tok) {
	char* printList[] = {
		"END_OF_FILE",// -1
		"IDENTIFIER",  // 0
		"ERROR_TOKEN", // 1
		"INTEGER_LITERAL", // 2
		"STRING_LITERAL", // 3
		"FLOAT_LITERAL", // 4
		"CHAR_LITERAL", // 5
		"TYPE_DECLARE", // 6
		"OPERATOR", // 7
		"BRACKETS", // 8
		"LOGIC_OPERATORS", //9
		"WHILE_STATEMENT",  //10
		"OPEN_PARENTHESES",  // 11
		"CLOSED_PARENTHESESE", //12
		
		"BUILT_IN_FUNCTION", //14
		"MODIFIER",  //15
		"STATEMENT_START",  //16
		"STATEMENT_END", //17
		"END_OF_LINE", //18
		"SEPARATOR", //19
		"COMMAND_ADD",
		"PRIMARY_CONDITION",
		"SECONDARY_CONDITION",
		"FINAL_CONDITION" };
}


void printParseTree(parseTree* node, int depth, int lastChild) {
    if (node == NULL)
        return;

    // Print dashes to represent hierarchy
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1)
            printf("%s", "|---");
        else
            printf("   ");
    } 

    // Print the value of the current node
    printf("%s", node->val->id);
	if (node->val->tok == OPERATOR) {
		printf(" : (%d)", node->val->precedence);
	}
	printf("\n");

    // Recursively print the children of the current node
    if (node->son != NULL)
        printParseTree(node->son, depth + 1, node->brother == NULL);

    // Recursively print the brothers of the current node
    if (node->brother != NULL) 
        printParseTree(node->brother, depth, 1);

}


void main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("please enter a file path!");
		return;

	}
	char* errorSyntax = _strdup("\0");
	int errorSyntaxSize = 1;
	char* tempAllocate = NULL;
	createAutomation();
	initFile(argv[1]);
	Node* tokList = createTokenList();	
	Token help;
	char* lineSaver;
	int spaceNeeded = 0;
	printLinkedList(tokList, printToken);
	createPDA();
	parseTree* x = callPda(&tokList);
    parseTree* temp = x;
    parseTree* xCursor = x;
	int f = 1;
	if (!temp) {
		if (tokList) {
			spaceNeeded = snprintf(NULL, 0, "line :%d;    ", ((TokenId*)tokList->data)->line);
			lineSaver = (char*)malloc(spaceNeeded + 1);
			sprintf(lineSaver, "line: %d:   ", ((TokenId*)tokList->data)->line);
			errorSyntaxSize += (int)strlen(lineSaver) + (int)strlen("\nnot according to syntax, error in:  ");
			tempAllocate = (char*)realloc(errorSyntax, sizeof(char) * errorSyntaxSize);
			if (tempAllocate)
				errorSyntax = tempAllocate;
			else
				return;
			strcat(errorSyntax, "\nnot according to syntax, error in:  ");
			strcat(errorSyntax, lineSaver);
		}
		

		while (tokList && tokList->data && (((TokenId*)tokList->data)->tok != END_OF_LINE && ((TokenId*)tokList->data)->tok != END_OF_FILE))
		{
			
			
			errorSyntaxSize += 4 + (int)strlen(((TokenId*)tokList->data)->id);
			tempAllocate = (char*)realloc(errorSyntax, sizeof(char) * errorSyntaxSize);
			if (tempAllocate)
				errorSyntax = tempAllocate;
			else
				return;
			
			strcat(errorSyntax, ((TokenId*)tokList->data)->id);
			strcat(errorSyntax, " ");
			tokList = tokList->next;
		}
	}
	while ((temp && temp->val && temp->val->tok != END_OF_FILE) || temp == NULL) {
        printf("\n\n");
		if (tokList) {
			tokList = tokList->next;
			if (tokList && tokList->data)
				help = ((TokenId*)tokList->data)->tok;
			temp = callPda(&tokList);
			if (temp) {
				if (xCursor) {
					xCursor->brother = temp;
					xCursor = xCursor->brother;
				}
				else {
					x = temp;
					xCursor = x;
				}

			}
			else {
				if (tokList) {
					spaceNeeded = snprintf(NULL, 0, "line :%d;    ", ((TokenId*)tokList->data)->line);
					lineSaver = (char*)malloc(spaceNeeded + 1);
					sprintf(lineSaver, "line: %d:   ", ((TokenId*)tokList->data)->line);
					errorSyntaxSize += (int)strlen(lineSaver) + (int)strlen("\nnot according to syntax, error in:  ");
					tempAllocate = (char*)realloc(errorSyntax, sizeof(char) * errorSyntaxSize);
					if (tempAllocate)
						errorSyntax = tempAllocate;
					else
						return;
					strcat(errorSyntax, "\nnot according to syntax, error in:  ");
					strcat(errorSyntax, lineSaver);
				}
				
				
				while (tokList && tokList->data && (((TokenId*)tokList->data)->tok != END_OF_LINE && ((TokenId*)tokList->data)->tok != END_OF_FILE)) {
					errorSyntaxSize += 3 + (int)strlen(((TokenId*)tokList->data)->id);
					tempAllocate = (char*)realloc(errorSyntax, sizeof(char) * (errorSyntaxSize+3));
					if (tempAllocate)
						errorSyntax = tempAllocate;
					else
						return;
					strcat(errorSyntax, ((TokenId*)tokList->data)->id);
					strcat(errorSyntax, " ");
					tokList = tokList->next;
				}
				errorSyntaxSize += sizeof(char) * (int)(1 + strlen("\n"));
				tempAllocate = (char*)realloc(errorSyntax, errorSyntaxSize);
				if (tempAllocate) {
					errorSyntax = tempAllocate;
					//strcat(errorSyntax, "\n");

				}
				else {
					printf("unexpected error\n");
					return;
				}
				


			}

		}
		else
			temp = ParseTreeSetVal(createTokenId(END_OF_FILE, "", 0));
		
      
	}
     printParseTree(x, 0, 0);

	
	removeParenthesis(x, 0);
	checkExpressionsVars(x);
	if (errorSyntaxSize == 1 && isNoErrors()) {
		checkDeclaration(x);
	}

	if (errorSyntaxSize == 1 && isNoErrors()) {
		changePrecedence(&x);
		printParseTree(x, 0, 0);
		handleAsm(x);
	}
	else {
		printf(errorSyntax);
		printf("\n");
		if(errorSyntaxSize == 1)
			printErrors();
		
	}



}