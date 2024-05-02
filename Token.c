#define _CRT_SECURE_NO_WARNINGS
#include "Tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include "FileAbstraction.h"
#include "Automate.h"
#include "LinkedList.h"
#include "hash.h"
#include <string.h>
#include "Stack.h"
#include "scope.h"



TokenId* createTokenId(Token Tok, char* id, int precednce) {
	TokenId* ret = (TokenId*)malloc(sizeof(TokenId));
	if (!ret) {
		printf("an error has occurred\n");
		return NULL;
	}
	ret->id = id;
	ret->precedence = precednce;
	ret->tok = Tok;
	return ret;

}



TokenId* nextTokenId() {
	int i = 0;
	char letter = nextLetter();
	char* res = (char*)malloc(sizeof(char));
	int noNeedToSeparate = 0;
	char* tempAlloc = NULL;
	TokenId* ret = NULL;
	if (!res)
		return NULL;
	if (letter == '\"')
		noNeedToSeparate = 1;
	if (letter == ' ' || letter == '\n' || letter == EOF || letter == '\t') {
		res[i] = letter;
		i++;
		tempAlloc = (char*)realloc(res, sizeof(char) * (2));
		if (tempAlloc)
			res = tempAlloc;
		else
			return NULL;
		*(res+1) = '\0';
		if (letter == '\n')
			ret = createTokenId(END_OF_LINE, res, 0);
		else if (letter == ' ' || letter == '\t')
			ret = createTokenId(SEPARATOR, res, 0);
		else
			ret = createTokenId(END_OF_FILE, res, 0);
		return ret;
	}
	i = 0;
	while (letter != '\0' && (letter != ' ' || letter == ' ' && noNeedToSeparate
		&& letter != '\"') && letter != '\n' && letter != EOF) {
		res[i] = letter;
		i++;
		tempAlloc = (char*)realloc(res, sizeof(char) * (i + 1));
		if (tempAlloc)
			res = tempAlloc;
		else
			return NULL;
		if (!res || letter < 0 || letter > 127)
			return NULL;
		letter = nextLetter();
	}
	if(letter!=EOF )
		revertFile(1);
	if (letter == '\n')
		revertFile(1);
	res[i] = '\0';
	ret = tokenize(res);
	if (ret == NULL) {
		ret = createTokenId(IDENTIFIER, res, 0);
	}
	return ret;
}

void addToString(char** word, int num) {
	char* tempAlloc = NULL;
	int size;
	int x = num;
	int numDigits = 0;
	if (x == 0)
		numDigits = 1;
	else {
		while (x != 0) {
			x /= 10;
			numDigits++;
		}
	}
	size = (int)strlen(*word) / sizeof(char) + numDigits + 3;
	tempAlloc = (char*)realloc(*word, size);
	if (tempAlloc)
		*word = tempAlloc;
	else
		return NULL;
	sprintf(*word,  "%s%d", *word, num);
}



Node* createTokenList() {
	initScopeArr();
	int numBrothers = 0;
	Stack* pathNum = stackCreate(sizeof(int));
	push(pathNum, &numBrothers);
	char* scopeID = _strdup("0");
	addScope(scopeID);
	int start = 0;
	int line = 0;
	Node* tokenList = NULL;
	Node* startTokens = NULL;
	TokenId* curr = nextTokenId();
	char* tempAlloc = NULL;
	while (curr->tok != EOF) {
		curr->line = line;
		if (curr->tok == STATEMENT_START) {
			numBrothers = *(int*)top(pathNum) + 1;
			start = 0;
			push(pathNum, &start);
			tempAlloc = (char*)realloc(scopeID, strlen(scopeID) + 5);
			if (tempAlloc) {
				scopeID = tempAlloc;

			}
			else
				return NULL;
			 
			strcat(scopeID, ".");
			addToString(&scopeID, numBrothers);
			addScope(scopeID);

		}
		else if (curr->tok == STATEMENT_END) {
			pop(pathNum);
			numBrothers = *(int*)pop(pathNum) + 1;
			push(pathNum, &numBrothers);
			saveUntilChr(&scopeID, '.');
			addScope(scopeID);
		}
		else if (curr->tok == IDENTIFIER) {
			if (symbolGet(curr->id, scopeID) == NULL) {
				tableInsert(createEmptyIdentifer(curr->id, _strdup(scopeID),line,getScopesValuesSize() - 1));
				curr->scopeIndex = getScopesValuesSize() - 1;
			}
			else {
				curr->scopeIndex = symbolGet(curr->id, scopeID)->scopeIndex;
			}
			curr->scope = _strdup(scopeID);
			
			
		}
		else if (curr->tok == END_OF_LINE) {
			line++;
		}
		if (curr->tok != SEPARATOR) {
			insertAtEnd(&tokenList, curr);
			if (!startTokens)
				startTokens = tokenList;
			else
				tokenList = tokenList->next;
		}
		curr = nextTokenId();
	}
	insertAtEnd(&tokenList, curr);
	if (!startTokens)
		startTokens = tokenList;
	return startTokens;
}

void printToken(TokenId* tok) {
	if(tok->tok != SEPARATOR)
		printf("%s : %d : %d\n", tok->id, tok->tok,tok->line);
}


