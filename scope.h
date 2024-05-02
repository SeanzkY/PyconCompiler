#pragma once


typedef struct {
	char** scopes;
	int size;
	int currValues;

}scopeArr;

int getScopesValuesSize();

void initScopeArr();

void addScope(char* value);



char* findInsideScope(int index);