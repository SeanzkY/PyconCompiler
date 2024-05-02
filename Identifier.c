#include "Identifier.h"
#include <stdlib.h>
#include <stdio.h>



IdentiferType* createEmptyIdentifer(char* name,char* scope,int line,int scopeIndex) {
	IdentiferType* ret = (IdentiferType*)malloc(sizeof(IdentiferType));
	if (!ret)
		return ret;
	ret->name = name;
	ret->line = line;
	ret->scope = scope;
	ret->isSet = 0;
	ret->isFunction = 0;
	ret->scopeIndex = scopeIndex;
	return ret;
}




int typeMatch(char* type, char* value);