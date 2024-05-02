#include "parserAutomation.h"
#include <stdlib.h>
#include <stdio.h>
#include "Stack.h"
#include "FileAbstraction.h"
#include "Tokens.h"
#include <string.h>
#include "parser.h"
#include "hash.h"
#include "scope.h"

PDA* pdaList;


// declare id for each non terminal
#define declarationIntModeID 0
#define expressionModeID 1
#define functionModeID 2
#define statementModeID 3
#define conditionModeID 4
#define whileModeID 5
#define assignmentModeID 6
#define expressionContModeId 7
#define declarationStringModeID 8
#define printVarModeID 9


// set value for tree
parseTree* ParseTreeSetVal(TokenId* val) {
	parseTree* t;
	t = (parseTree*)malloc(sizeof(parseTree));
	(t)->son = NULL;
	(t)->brother = NULL;
	(t)->val = val;

	return t;
}


// init the automation
void initPda()
{
	int i, j;
	pdaList = (PDA*)malloc(sizeof(PDA));
	for (i = 0; i < numMode; i++) {
		// init all modes
		for (j = 0; j < maxModeLen; j++) {
			pdaList->arr[i][j] = (modeArr*)malloc(sizeof(modeArr));
			pdaList->arr[i][j]->modeLen = 0;
			pdaList->arr[i][j]->currMode = NULL;
		}
	}
	
}

nextModeConnection* createModeConnection(Token nextMode, int nextModeIndex, int nextModeId) {
	nextModeConnection* result = (nextModeConnection*)malloc(sizeof(nextModeConnection));
	result->nextMode = nextMode;
	result->nextModeIndex = nextModeIndex;
	result->nextModeId = nextModeId;
	return result;
}




void addRoute(modeRepresent** lst, int size) {
	int i;
	// this is just to make the code more clear - represent the list of modes in one item of the matrix
	modeArr* lstMode;
	for (i = 0; i < size; i++) {
		// if last value put -1 else put next value
		// add arrow to mode
		lstMode = pdaList->arr[(*lst)->modeIndex][i];
		lstMode->currMode = (mode**)realloc(lstMode->currMode, sizeof(mode*) * ++lstMode->modeLen);
		lstMode->currMode[lstMode->modeLen - 1] = (mode*)malloc(sizeof(mode));
		lstMode->currMode[lstMode->modeLen - 1]->modeId = (*lst)->currMode->modeId;
		lstMode->currMode[lstMode->modeLen - 1]->modeConnections = NULL;
		lstMode->currMode[lstMode->modeLen - 1]->nonTerminal = (*lst)->currMode->nonTerminal;
		// insertions of all connections to mode
		Node* saveLst = (*lst)->currMode->modeConnections;
		while (saveLst) {
			insertAtBeginning(&(lstMode->currMode[lstMode->modeLen - 1]->modeConnections), saveLst->data);
			saveLst = saveLst->next;
		}
		lst++;
	}
}

// find mode index for a mode pda (usually 0)
int findPDAMode(mode** lst, int mode,int size) {
	int i = 0;
	if (!lst)
		return -1;
	for(i=0;i<size;i++) {
		if (mode == (*lst)->modeId)
			return i;
		lst++;
	}
	return -1;
}

int stackTopEqual(char val, Stack* s) {
	return (stackIsEmpty(s) && val == -1) || (!stackIsEmpty(s) && *(char*)(top(s)) == val);
}


// find all the suitable next mode connection in one mode
nextModeConnection* findConn(modeArr* lst, int modeID,Token nextToken) {
	if (lst == NULL)
		return NULL;
	
	int modeIndex = findPDAMode(lst->currMode, modeID, lst->modeLen);
	if (modeIndex == -1)
		return NULL;
	nextModeConnection* conn;
	Node* connList = lst->currMode[modeIndex]->modeConnections;
	while (connList) {
		conn = (nextModeConnection*)connList->data;
		if (conn->nextModeId == -1 ||  conn->nextMode == nextToken)
			return conn;
		connList = connList->next;
	}
	return NULL;
}

// check if identifiers are valid when they appear 
int checkIdentifiers(TokenId* t, int isDeclaration,Token currType) {
	if (isDeclaration) {
		return declareIdentifier(t,currType);

	}
	else {
		return isDeclaredIdentifier(t);
	}
}

// for implement function paramaters - this is for future creation not for the schedule being
void handleFunctionParamaters(TokenId* identifier) {
	IdentiferType* res = symbolGet(identifier->id, identifier->scope);
	if (res->line == identifier->line) {
		tableDelete(identifier);
		res->scope = _strdup(findInsideScope(identifier->scopeIndex));
		res->scopeIndex++;
		identifier->scopeIndex++;
		tableInsert(res);
		identifier->scope = res->scope;
	}
}
void handleFunctionHead(TokenId* identifier) {
	IdentiferType* res = symbolGet(identifier->id, identifier->scope);
	if (res->line == identifier->line) {
		res->isFunction = 1;
	}
	else {
		res = createEmptyIdentifer(identifier->id, identifier->scope, identifier->line, identifier->scopeIndex+1);
		res->isFunction = 1;
		tableInsert(res);
		
	}
}

parseTree* passPDATry(Node** list, int modeID, int startIndex,int skip) {
	int isDeclaration = 0;
	int isFunctionHead = 0;
	parseTree* parseTreeStart = NULL;
	parseTree* parseTreeCursor = NULL;
	parseTree* treeHelper = NULL;
	nextModeConnection* conn;
	int passMode = 1,i=startIndex;
	modeArr* lstMode = NULL;
	Node* tokenList = *list;
	Node* helper;
	Token currType = END_OF_FILE;
	Token currToken = ((TokenId*)tokenList->data)->tok;
	lstMode = pdaList->arr[currToken][i];
	conn = findConn(lstMode, modeID, ((TokenId*)tokenList->next->data)->tok);
	if (conn == NULL || currToken == END_OF_LINE || currToken == END_OF_FILE) {
		if (skip)
			return NULL;
		parseTreeStart = ParseTreeSetVal((TokenId*)tokenList->data);
		parseTreeCursor = parseTreeStart;
		if (currToken == IDENTIFIER &&
			!checkIdentifiers((TokenId*)tokenList->data, isDeclaration, currType)) {
			// ask to stop
		}
		
	

	}
	while (conn!=NULL) {
		if (modeID == functionModeID && currToken == IDENTIFIER) {
			if (!isFunctionHead) {
				isFunctionHead = 1;
				handleFunctionHead((TokenId*)tokenList->data);
			}
			else {
				handleFunctionParamaters((TokenId*)tokenList->data);
			}
		}
		if (currToken == TYPE_INTEGER_DECLARE || currToken == TYPE_STRING_DECLARE) {
			currType = currToken;
			isDeclaration = 1;

		}
		else if (currToken == IDENTIFIER &&
			!checkIdentifiers((TokenId*)tokenList->data, isDeclaration,currType)) {
			// ask to stop-
		}
		else
			isDeclaration = 0;
 		currToken = ((TokenId*)tokenList->data)->tok;
		// need skip if we already checked one token
		// we need to skip this entirely if it's not the same non terminal because we dont want duplicates in this situation
		if (skip && conn && conn->nextModeId != modeID && conn->nextModeId != -1) {

		}
		else {
			if (!parseTreeStart) {
				if (!skip)
					parseTreeStart = ParseTreeSetVal((TokenId*)tokenList->data);
				else
					parseTreeStart = ParseTreeSetVal((TokenId*)tokenList->next->data);

				parseTreeCursor = parseTreeStart;
			}
			else {
				if (!skip)
					parseTreeCursor->brother = ParseTreeSetVal((TokenId*)tokenList->data);
				else
					parseTreeCursor->brother = ParseTreeSetVal((TokenId*)tokenList->next->data);
				parseTreeCursor = parseTreeCursor->brother;
			}
		}
		
		passMode = 1;
		tokenList = tokenList->next;
		if(conn){
			// any non terminal
			if (conn->nextModeId == -1) {
				i = conn->nextModeIndex;
				tokenList = tokenList->next;
				while (!(conn->nextMode == ((TokenId*)tokenList->data)->tok)) {
					if (((TokenId*)(tokenList->data))->tok == END_OF_LINE)
						tokenList = tokenList->next;
					parseTree* result = callPda(&tokenList);
					if (result == NULL)
						return NULL;
					else {
						//add
						parseTreeCursor->brother = result;
						parseTreeCursor = parseTreeCursor->brother;
						tokenList = tokenList->next;
						
					}
				}
				parseTreeCursor->brother =  ParseTreeSetVal((TokenId*)tokenList->data);
				parseTreeCursor = parseTreeCursor->brother;
				skip = 1;
			}
			// same non terminal
			else if (conn->nextModeId == modeID) {
				i = conn->nextModeIndex;
			}
			// diferent non terminal
			else {
				helper = tokenList;
				while (conn && conn->nextModeId != modeID) {
					parseTreeCursor->brother = passPDATry(&(helper), conn->nextModeId, conn->nextModeIndex,0);
					tokenList = helper;
					
					if (parseTreeCursor->brother == NULL && !skip)
						return NULL;
						currToken = ((TokenId*)tokenList->data)->tok;
					parseTreeCursor = parseTreeCursor->brother;
					// goto next command
					lstMode = pdaList->arr[END_OF_LINE][i + 1];
					i++;
					if (tokenList->next != NULL) {
					
						// find next connection
						conn = findConn(lstMode, modeID, ((TokenId*)tokenList->next->data)->tok);
						helper = helper->next;
					}
					else
						conn = NULL;
					
				}
				// continue on the track before
				if (conn) {
					skip = 1;
					passMode = 0;
				}
				else
					i = -1;
			}
		}
		if (i != -1) {
			if (passMode)
				currToken = ((TokenId*)tokenList->data)->tok;
			else
				currToken = END_OF_LINE;
			lstMode = pdaList->arr[currToken][i];
		
			conn = findConn(lstMode, modeID, ((TokenId*)tokenList->next->data)->tok);
		}
		else {
			conn = NULL;
		}
	}
	treeHelper = ParseTreeSetVal(lstMode->currMode[findPDAMode(lstMode->currMode, modeID, lstMode->modeLen)]->nonTerminal);
	treeHelper->son = parseTreeStart;
	if (treeHelper->val == NULL)
		return NULL;
	*list = tokenList;
	return treeHelper;
	


}



parseTree* callPda(Node** list) {
	hashTable restore;
    Node* tokenList = *list;
	Node* savePos = tokenList;
	parseTree* result ;
	if (!tokenList)
		return NULL;
	Token currToken = ((TokenId*)tokenList->data)->tok;
	while (currToken == END_OF_LINE) {
		*list = tokenList->next;
		tokenList = tokenList->next;
		currToken = ((TokenId*)tokenList->data)->tok;
	}
	if (currToken == END_OF_FILE) {
		result = ParseTreeSetVal((TokenId*)tokenList->data);
		return result;

	}
	modeArr* lstMode = NULL;
	// conn and connections are just the arrows
	int i;
	lstMode = pdaList->arr[currToken][0];
	for (i = 0; i < lstMode->modeLen; i++) {
		cpyTable(&restore);
		// if the grammar can be interpeted in multiple ways for the first try again just for this to still work
		result = passPDATry(list,lstMode->currMode[i]->modeId,0,0);
		if (result != NULL && result->val != NULL && strcmp(result->val->id,"expression") != 0 )
			return result;
		else {
			*list = tokenList;
			restoreHashTable(restore);
		}
	}
	return NULL;

	
}






modeRepresent* createModeRepresent(Token currMode, nextModeConnection** modeInstructions,
	int sizeInstructions,int modeID, TokenId* nonTerminal) {
	int i;
	modeRepresent* result = (modeRepresent*)malloc(sizeof(modeRepresent));
	result->modeIndex = currMode;
	result->currMode = (mode*)malloc(sizeof(mode));
	result->currMode->modeConnections = NULL;
	result->currMode->modeId = modeID;
	result->currMode->nonTerminal = nonTerminal;
	for (i = 0; i < sizeInstructions; i++) {
		insertAtBeginning(&(result->currMode->modeConnections), modeInstructions[i]);
	}
	return result;
}

void createDeclaration() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "declaration";
	nextModeConnection* connectionsLst[] = { createModeConnection(IDENTIFIER,1,declarationIntModeID) };
	nextModeConnection* connectionsLst1[] = { 
		createModeConnection(MODIFIER,3,declarationIntModeID)
	};
	
	nextModeConnection* connectionsLst2[] = {
		createModeConnection(INTEGER_LITERAL,0,expressionModeID),
		createModeConnection(IDENTIFIER,0,expressionModeID),
		createModeConnection(OPEN_PARENTHESES,0,expressionModeID)
	};
	nextModeConnection* connectionsLst3[] = { createModeConnection(-1,-1,expressionModeID) };

	nextModeConnection* connectionsLst8[] = { createModeConnection(END_OF_LINE,4,declarationIntModeID) };
	// function is not null = accepting mode
	// 
	// declaration
	modeRepresent* lst[] = {
		  createModeRepresent(TYPE_INTEGER_DECLARE, connectionsLst,sizeof(connectionsLst) / sizeof(nextModeConnection*),declarationIntModeID,NULL)
		, createModeRepresent(IDENTIFIER, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),declarationIntModeID,NULL)
		, createModeRepresent(END_OF_LINE, NULL,0,declarationIntModeID,tok)
		, createModeRepresent(MODIFIER,connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),declarationIntModeID,NULL)
		// declares the function to do after switch id
		, createModeRepresent(END_OF_LINE,connectionsLst8,sizeof(connectionsLst8) / sizeof(nextModeConnection*),declarationIntModeID,tok)
	}; 

	nextModeConnection* connectionsLst4[] = { createModeConnection(IDENTIFIER,1,declarationStringModeID) };
	nextModeConnection* connectionsLst5[] = {
		createModeConnection(MODIFIER,3,declarationStringModeID)
	};
	nextModeConnection* connectionsLst6[] = { createModeConnection(STRING_LITERAL,4,declarationStringModeID),
	createModeConnection(IDENTIFIER,5,declarationStringModeID) };


	nextModeConnection* connectionsLst7[] = { createModeConnection(END_OF_LINE,2,declarationStringModeID) };

	modeRepresent* lst1[] = {
		  createModeRepresent(TYPE_STRING_DECLARE, connectionsLst4,sizeof(connectionsLst4) / sizeof(nextModeConnection*),declarationStringModeID,NULL)
		, createModeRepresent(IDENTIFIER, connectionsLst5,sizeof(connectionsLst5) / sizeof(nextModeConnection*),declarationStringModeID,NULL)
		, createModeRepresent(END_OF_LINE, NULL,0,declarationStringModeID,tok)
		, createModeRepresent(MODIFIER,connectionsLst6,sizeof(connectionsLst6) / sizeof(nextModeConnection*),declarationStringModeID,NULL)
		, createModeRepresent(STRING_LITERAL,connectionsLst7,sizeof(connectionsLst7) / sizeof(nextModeConnection*),declarationStringModeID,tok)
		, createModeRepresent(IDENTIFIER,connectionsLst7,sizeof(connectionsLst7) / sizeof(nextModeConnection*),declarationStringModeID,tok)
	};


	addRoute(lst, sizeof(lst) / sizeof(*lst));
	addRoute(lst1, sizeof(lst1) / sizeof(*lst1));
}


// function for creating expression
void createExpression()
{
	// this really just physically build an automation for expression like for other stuff I did her - following stricly the rules of bnf
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "expression";
	nextModeConnection* connectionsLst1[] = {
		createModeConnection(OPERATOR,1,expressionModeID)
	};
	nextModeConnection* connectionsLst2[] = {
		createModeConnection(INTEGER_LITERAL,0,expressionContModeId),
		createModeConnection(IDENTIFIER,0,expressionContModeId),
		createModeConnection(OPEN_PARENTHESES,0,expressionContModeId)

	};
	nextModeConnection* connectionsLst3[] = {
		createModeConnection(CLOSED_PARENTHESESE,2,expressionModeID)
	};
	modeRepresent* lst[] = {
		createModeRepresent(INTEGER_LITERAL, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),expressionModeID,tok),
		createModeRepresent(OPERATOR, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),expressionModeID,NULL),
		createModeRepresent(END_OF_LINE,NULL,0,expressionModeID,tok)
	};

	modeRepresent* lst1[] = {
		createModeRepresent(IDENTIFIER, connectionsLst1, sizeof(connectionsLst1) / sizeof(nextModeConnection*), expressionModeID,tok)
	};
	modeRepresent* lst2[] = {
	createModeRepresent(OPEN_PARENTHESES, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*), expressionModeID,NULL),
	createModeRepresent(END_OF_LINE,connectionsLst3,sizeof(connectionsLst3) / sizeof(nextModeConnection*),expressionModeID,NULL),
	createModeRepresent(CLOSED_PARENTHESESE, connectionsLst1, sizeof(connectionsLst1) / sizeof(nextModeConnection*), expressionModeID,tok)

	};
	addRoute(lst, sizeof(lst) / sizeof(*lst));
	addRoute(lst1, sizeof(lst1) / sizeof(*lst1));
	addRoute(lst2, sizeof(lst2) / sizeof(*lst2));
}

// helper function for creating expression

void createExpressionFix() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "expression";
	nextModeConnection* connectionsLst1[] = {
		createModeConnection(OPERATOR,1,expressionContModeId),
	};
	nextModeConnection* connectionsLst2[] = {
		createModeConnection(INTEGER_LITERAL,0,expressionModeID),
		createModeConnection(IDENTIFIER,0,expressionModeID),
		createModeConnection(OPEN_PARENTHESES,0,expressionModeID)
	};
	nextModeConnection* connectionsLst3[] = {
		createModeConnection(CLOSED_PARENTHESESE,2,expressionContModeId)
	};
	modeRepresent* lst[] = {
		createModeRepresent(INTEGER_LITERAL, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),expressionContModeId,tok),
		createModeRepresent(OPERATOR, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),expressionContModeId,NULL),
		createModeRepresent(END_OF_LINE,NULL,0,expressionContModeId,tok)
	};
	modeRepresent* lst1[] = {
		createModeRepresent(IDENTIFIER, connectionsLst1, sizeof(connectionsLst1) / sizeof(nextModeConnection*), expressionContModeId,tok)
	};
	modeRepresent* lst2[] = {
	createModeRepresent(OPEN_PARENTHESES, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*), expressionContModeId,NULL),
	createModeRepresent(END_OF_LINE,connectionsLst3,sizeof(connectionsLst3) / sizeof(nextModeConnection*),expressionContModeId,NULL),
	createModeRepresent(CLOSED_PARENTHESESE, connectionsLst1, sizeof(connectionsLst1) / sizeof(nextModeConnection*), expressionContModeId,tok)

	};

	addRoute(lst, sizeof(lst) / sizeof(*lst));
	addRoute(lst1, sizeof(lst1) / sizeof(*lst1));
	addRoute(lst2, sizeof(lst2) / sizeof(*lst2));
}

void createFunction() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "function";
	nextModeConnection* connectionsLst1[] = {
		createModeConnection(IDENTIFIER,1,functionModeID)
	};
	nextModeConnection* connectionsLst2[] = {
		createModeConnection(OPEN_PARENTHESES,2,functionModeID)
	};
	nextModeConnection* connectionsLst3[] = {
		createModeConnection(CLOSED_PARENTHESESE,3,functionModeID),
		createModeConnection(TYPE_INTEGER_DECLARE,6,functionModeID)
	};
	nextModeConnection* connectionsLst4[] = {
	createModeConnection(END_OF_LINE,4,functionModeID)
	};
	nextModeConnection* connectionsLst5[] = {
	createModeConnection(STATEMENT_START,0,statementModeID)
	};
	nextModeConnection* connectionsLst6[] = {
	createModeConnection(IDENTIFIER,7,functionModeID)

	};
	nextModeConnection* connectionsLst7[] = {
		createModeConnection(CLOSED_PARENTHESESE,3,functionModeID),
		createModeConnection(COMMAND_ADD,8,functionModeID)
	};
	nextModeConnection* connectionsLst8[] = {
	createModeConnection(TYPE_INTEGER_DECLARE,6,functionModeID)
	};
	modeRepresent* lst[] = {
			createModeRepresent(TYPE_INTEGER_DECLARE, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(IDENTIFIER, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(OPEN_PARENTHESES, connectionsLst3,sizeof(connectionsLst3) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(CLOSED_PARENTHESESE, connectionsLst4,sizeof(connectionsLst4) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst5,sizeof(connectionsLst5) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(END_OF_LINE,NULL,0,functionModeID,tok),
			createModeRepresent(TYPE_INTEGER_DECLARE, connectionsLst6,sizeof(connectionsLst6) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(IDENTIFIER, connectionsLst7,sizeof(connectionsLst7) / sizeof(nextModeConnection*),functionModeID,NULL),
			createModeRepresent(COMMAND_ADD, connectionsLst8,sizeof(connectionsLst8) / sizeof(nextModeConnection*),functionModeID,NULL)
	};
	addRoute(lst, sizeof(lst) / sizeof(*lst));
}


void createStatemenet() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "statement";
	nextModeConnection* connectionsLst1[] = {
		// if u put -1 in mode it means that there is something before (declaration statement or anything)
		createModeConnection(STATEMENT_END,1,-1)
	};
	nextModeConnection* connectionsLst2[] = {
	createModeConnection(END_OF_LINE,2,statementModeID)
	};
	modeRepresent* lst[] = {
			createModeRepresent(STATEMENT_START, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),statementModeID,NULL),
			createModeRepresent(STATEMENT_END, connectionsLst2 ,sizeof(connectionsLst2) / sizeof(nextModeConnection*),statementModeID,NULL),
			createModeRepresent(END_OF_LINE, NULL ,0,statementModeID,tok)
	};
	addRoute(lst, sizeof(lst) / sizeof(*lst));
}
void createCondition() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "condition";
	nextModeConnection* connectionsLst1[] = {
		createModeConnection(OPEN_PARENTHESES,0,expressionModeID)
	};
	nextModeConnection* connectionsLst2[] = {
		createModeConnection(STATEMENT_START,0,statementModeID),
	   
	};
	nextModeConnection* connectionsLst3[] = {
		createModeConnection(END_OF_LINE,4,conditionModeID),

	};

	nextModeConnection* connectionsLst4[] = {
		createModeConnection(SECONDARY_CONDITION,6,conditionModeID),
		createModeConnection(FINAL_CONDITION,3,conditionModeID)
	};

	modeRepresent* lst[] = {
			createModeRepresent(PRIMARY_CONDITION, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),conditionModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),conditionModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst4,sizeof(connectionsLst4) / sizeof(nextModeConnection*),conditionModeID,tok),
			createModeRepresent(FINAL_CONDITION, connectionsLst3,sizeof(connectionsLst3) / sizeof(nextModeConnection*),conditionModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),conditionModeID,NULL),
			createModeRepresent(END_OF_LINE, NULL,0,conditionModeID,tok),
			createModeRepresent(SECONDARY_CONDITION, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),conditionModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),conditionModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst4,sizeof(connectionsLst4) / sizeof(nextModeConnection*),conditionModeID,tok)
			 
	};
	
	addRoute(lst, sizeof(lst) / sizeof(*lst));

}

void createWhile() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "while";
	nextModeConnection* connectionsLst1[] = {
		createModeConnection(OPEN_PARENTHESES,0,expressionModeID)
	};
	nextModeConnection* connectionsLst2[] = {
		createModeConnection(STATEMENT_START,0,statementModeID)

	};
	modeRepresent* lst[] = {
			createModeRepresent(WHILE_STATEMENT, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),whileModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),whileModeID,NULL),
			createModeRepresent(END_OF_LINE, NULL,0,whileModeID,tok)

	};
	addRoute(lst, sizeof(lst) / sizeof(*lst));

}

void createAssignment() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "assignment";
	nextModeConnection* connectionsLst1[] = {
	createModeConnection(MODIFIER,1,assignmentModeID)
	};
	nextModeConnection* connectionsLst2[] = {
	createModeConnection(IDENTIFIER,0,expressionModeID),
	createModeConnection(INTEGER_LITERAL,0,expressionModeID),
	createModeConnection(OPEN_PARENTHESES,0,expressionModeID),
	createModeConnection(STRING_LITERAL,3,assignmentModeID)

	};
	nextModeConnection* connectionsLst3[] = {
		createModeConnection(END_OF_LINE,2,assignmentModeID)
	};
	nextModeConnection* connectionsLst4[] = {
		createModeConnection(END_OF_LINE,2,assignmentModeID)
	};
	modeRepresent* lst[] = {
			createModeRepresent(IDENTIFIER, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),assignmentModeID,NULL),
			createModeRepresent(MODIFIER, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),assignmentModeID,NULL),
			createModeRepresent(END_OF_LINE, connectionsLst4,sizeof(connectionsLst4) / sizeof(nextModeConnection*),assignmentModeID,tok),
			createModeRepresent(STRING_LITERAL, connectionsLst3,sizeof(connectionsLst3) / sizeof(nextModeConnection*),assignmentModeID,NULL)
	};
	addRoute(lst, sizeof(lst) / sizeof(*lst));
}

void createPrint() {
	TokenId* tok = (TokenId*)malloc(sizeof(TokenId));
	tok->tok = 1;
	tok->id = "print";
	nextModeConnection* connectionsLst1[] = {
	createModeConnection(OPEN_PARENTHESES,1,printVarModeID)
	};
	nextModeConnection* connectionsLst2[] = {
	createModeConnection(IDENTIFIER,2,printVarModeID)

	};
	nextModeConnection* connectionsLst3[] = {
		createModeConnection(CLOSED_PARENTHESESE,3,printVarModeID)
	};
	nextModeConnection* connectionsLst4[] = {
		createModeConnection(END_OF_LINE,4,printVarModeID)
	};
	modeRepresent* lst[] = {
			createModeRepresent(BUILT_IN_FUNCTION, connectionsLst1,sizeof(connectionsLst1) / sizeof(nextModeConnection*),printVarModeID,NULL),
			createModeRepresent(OPEN_PARENTHESES, connectionsLst2,sizeof(connectionsLst2) / sizeof(nextModeConnection*),printVarModeID,NULL),
			createModeRepresent(IDENTIFIER, connectionsLst3,sizeof(connectionsLst3) / sizeof(nextModeConnection*),printVarModeID,NULL),
			createModeRepresent(CLOSED_PARENTHESESE, connectionsLst4,sizeof(connectionsLst4) / sizeof(nextModeConnection*),printVarModeID,NULL),
			createModeRepresent(END_OF_LINE, NULL,0,printVarModeID,tok)
	};
	addRoute(lst, sizeof(lst) / sizeof(*lst));
}


void createPDA() {
	// this just calls all the functions - the create function will not be used now
	initPda();
	createDeclaration();
	createExpression();
	createFunction();
	createStatemenet();
	createCondition();
	createWhile();
	createAssignment();
	createExpressionFix();
	createPrint();


	
	

	
}


