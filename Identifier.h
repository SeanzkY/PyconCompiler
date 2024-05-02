#include "Stack.h"
#include "Tokens.h"


typedef struct {
	Token type;
	char* name;
	// this is the value it might have
	int setValue;
	// this is the scope
	char* scope;
	// this means if it's declared already
	int isSet;
	int line;
	//this means if the identifier is a fucntion name or not
	int isFunction;
	// this is the index in the scope list
	int scopeIndex;

	char* codeName;
	int size;
}IdentiferType;


IdentiferType* createEmptyIdentifer(char* name, char* scope,int line,int scopeIndex);

IdentiferType* createIdentifier(char* name, char* type, int setValue,char* scope);



int typeMatch(char* type, char* value);