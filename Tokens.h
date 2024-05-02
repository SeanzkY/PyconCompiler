#pragma once
#define WHILE_KEYWORD "While"
#include "LinkedList.h"

// Possible tokens for each command - makes it much easier and more readable to parse
typedef enum {

	END_OF_FILE = -1,
	IDENTIFIER,  
	ERROR_TOKEN, 
	INTEGER_LITERAL, 
	STRING_LITERAL, 
	CHAR_LITERAL,
	TYPE_INTEGER_DECLARE, 
	OPERATOR, 
	BRACKETS, 
	LOGIC_OPERATORS, 
	WHILE_STATEMENT,  
	OPEN_PARENTHESES,  
	CLOSED_PARENTHESESE, 
	BOOLEAN_LITERAL, 
	BUILT_IN_FUNCTION, 
	MODIFIER,  
	STATEMENT_START, 
	STATEMENT_END,
	END_OF_LINE, 
	SEPARATOR, 
	TYPE_STRING_DECLARE, 
	COMMAND_ADD,
	PRIMARY_CONDITION,
	SECONDARY_CONDITION,
	FINAL_CONDITION
} Token;

// saves the necessary things for each token - the token itself (tok) which is just an enum classifying it, id - the actual
// name of the command, and precedence - which is only relevant if it's an operator (any kind)
typedef struct {
	Token tok;
	char* id;
	int precedence;
	char* scope;
	int line;
	int scopeIndex;
}TokenId;



// create a tokenId from the it's token,id and precedence
TokenId* createTokenId(Token Tok, char* id, int precednce);



// get the next tokenid in the file
TokenId* nextTokenId();

// prints token
void printToken(TokenId* tok);

// creates a linked list out of all the tokens
Node* createTokenList();








