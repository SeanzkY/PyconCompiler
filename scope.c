#include "scope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


scopeArr* scopeArrVar;

void initScopeArr() {
	scopeArrVar = (scopeArr*)malloc(sizeof(scopeArr));
	scopeArrVar->scopes = NULL;
	scopeArrVar->size = 0;
	scopeArrVar->currValues = 0;
}

int getScopesValuesSize() {
	return scopeArrVar->currValues;
}


void addScope(char* value) {
	scopeArrVar->currValues++;
	if (scopeArrVar->size < scopeArrVar->currValues) {
		scopeArrVar->size = scopeArrVar->size * 2 + 1;
		scopeArrVar->scopes = (char**)realloc(scopeArrVar->scopes, sizeof(char*) * scopeArrVar->size);
	}
	if (scopeArrVar->scopes == NULL)
		return NULL;
	(scopeArrVar->scopes)[scopeArrVar->currValues - 1] = _strdup(value);
}





char* findInsideScope(int index) {
	
	if (scopeArrVar->currValues <= index - 1)
		return NULL;
	return *(scopeArrVar->scopes + 1 + index);
}