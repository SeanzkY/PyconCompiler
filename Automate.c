#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Automate.h"
#include "Tokens.h"
#include <ctype.h>
#include "FileAbstraction.h"

// the automate itself
finiteAutomation* matrix[ASCI_TABLE_SIZE - UNNECESSARY_ASCII_VALS][MAX_MODES];




// find index for a mode using it's id
int findModeIndex(nextModePoint** lst,int mode,int size) {
	int i;
	for(i=0;i<size;i++){
		if ((*lst)->modeNum == mode) {
			return i;
		}
		lst++;
	
	}
	return -1;

}


TokenId* retToken(char* word,int nextLetter,int index,int mode, int prevModeIndex) {
	
	// for special case - one letters
	if (matrix[word[index - 1] - UNNECESSARY_ASCII_VALS][index - 1] == NULL ||
		matrix[word[index - 1] - UNNECESSARY_ASCII_VALS][index - 1]->next == NULL)
		return NULL;
	if (nextLetter == -1) {
		return matrix[word[index-1] - UNNECESSARY_ASCII_VALS][index-1]->next[prevModeIndex]->currToken;
	}
	if (!word[index] || matrix[word[index] - UNNECESSARY_ASCII_VALS][index] == NULL)
		return NULL;


	int modeIndex = findModeIndex(matrix[word[index] - UNNECESSARY_ASCII_VALS][index]->next, mode,
		matrix[word[index] - UNNECESSARY_ASCII_VALS][index]->nodesCount);
	if (modeIndex == -1)
		return NULL;
	if (word[index]) {
		if (nextLetter == word[index] - UNNECESSARY_ASCII_VALS ) {
			if (matrix[nextLetter][index]->next[modeIndex]->nextMode == -1) {
				// over or starts with no space char
				if (strlen(word) - 1 == index || matrix[nextLetter][index]->noSpaceChar) {
					return matrix[nextLetter][index]->next[modeIndex]->currToken;
				}
				// ends with no space char
				else if (matrix[word[index+1] - UNNECESSARY_ASCII_VALS][0] && matrix[word[index+1] - UNNECESSARY_ASCII_VALS][0]->noSpaceChar) {
					return matrix[nextLetter][index]->next[modeIndex]->currToken;
				}
				return NULL;
			}
			
			return retToken(word, matrix[nextLetter][index]->next[modeIndex]->nextMode, index + 1, mode,modeIndex);
		}
		
		
		return NULL;
	}
	return NULL;
	
}
TokenId* tokenize(char* word) {
	TokenId* ret;
	TokenId* temp;
	int i;
	finiteAutomation* helper;

	//special case empty
	if (*word==EOF)
		return createTokenId(END_OF_FILE, "", 0);

	// special case for numbers
	if (isdigit(*word)) {
		i = 1;
		while (word[i] && isdigit(word[i])) {
			i++;
		}
		if (isalpha(word[i]))
			return NULL;
		revertFile((int)(strlen(word) - (i)));
		word[i] = '\0';
		return createTokenId(INTEGER_LITERAL, word, 0);

	}
	// special case for string 
	if (*word == '\"') {
		i = 1;
		while (word[i] && word[i] != '\"') {
			i++;
		}
		if (word[i] == '\"') {
			revertFile((int)(strlen(word) - (i+1)));
			word[i + 1] = '\0';
			if (strcmp(word, "\"\\n\"") == 0) {
				free(word);
				word = _strdup("\n");
			}
			return createTokenId(STRING_LITERAL, word, 0);
		}
		return NULL;
	}
	if (word[0] - UNNECESSARY_ASCII_VALS < ASCI_TABLE_SIZE - UNNECESSARY_ASCII_VALS && word[0] - UNNECESSARY_ASCII_VALS >= 0) {
		helper = matrix[word[0] - UNNECESSARY_ASCII_VALS][0];
	}
	else {
		printf("letter not found!\n");
		helper = NULL;

	}
	

	// if it's not token we should format the identifier
	if (!helper) {
		i = 0;
		while (isalnum(word[i])) {
			i++;
		}
		revertFile((int)strlen(word)-(i));
		word[i] = '\0';
		return createTokenId(IDENTIFIER, word, 0);

	}

	// go through all the mode options of the first node
	for (i = 0; i < helper->nodesCount; i++) {
		ret = retToken(word, helper->next[i]->nextMode, 1, helper->next[i]->modeNum,i);
		if (ret) {
			revertFile((int)strlen(word) - (int)strlen(ret->id));
			temp = (TokenId*)malloc(sizeof(TokenId));
			memcpy(temp, ret, sizeof(TokenId));
			return temp;
		}
	}
	char* helperStr = strchr(word, '(');
	if (helperStr) {
		revertFile((int)strlen(word) + (int)strlen((char*)(word - strchr(word, '('))));
		word[strlen(word) + strlen((char*)(word - strchr(word, '(')))] = '\0';
	}
	
	
	return createTokenId(IDENTIFIER,word,0);
	
}


void addWord(char* word, int size, TokenId* token, int mode, int noSpaceChar) {
	int i,modesSize;
	// i - signals the index of the letter - both in the automation and in the word - represented by the second index
	for (i = 0; i < size; i++) {
		if (matrix[word[i] - UNNECESSARY_ASCII_VALS][i] == NULL) {
			modesSize = 1;
			matrix[word[i] - UNNECESSARY_ASCII_VALS][i] = (finiteAutomation*)malloc(sizeof(finiteAutomation));
			if (!matrix[word[i] - UNNECESSARY_ASCII_VALS][i])
				return;
			matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->nodesCount = modesSize;
			matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next = (nextModePoint**)malloc(sizeof(nextModePoint*));
		}
		else {
			// inc nodes count and save the increased value
			modesSize = ++(matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->nodesCount);

			matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next = 
				(nextModePoint**)realloc(matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next , modesSize * sizeof(nextModePoint*));
			if (matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next == NULL)
				return;
		}
		// put the char value
		// put the next node value
		if(matrix[word[i] - UNNECESSARY_ASCII_VALS][i] && matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next)
			matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1] = (nextModePoint*)malloc(sizeof(nextModePoint));
		if (i != size - 1) {
			if (matrix[word[i] - UNNECESSARY_ASCII_VALS][i] && matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next
				&& matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1])
				matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1]->nextMode =word[i + 1] - UNNECESSARY_ASCII_VALS;
		}
		else {
			// this marks the end of the word
			if (matrix[word[i] - UNNECESSARY_ASCII_VALS][i] && matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next
				&& matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1])
				matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1]->nextMode = -1;
		}
		// puts in the connect mode the token
		if (matrix[word[i] - UNNECESSARY_ASCII_VALS][i] && matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next
			&& matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1])
		{
			matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1]->currToken = token;
			// current mdoe
			matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->next[modesSize - 1]->modeNum = mode;
		}
			
		// "flag" - explained in struct
		matrix[word[i] - UNNECESSARY_ASCII_VALS][i]->noSpaceChar = noSpaceChar;

	}
}





void createAutomation() {
	char* typeInt[] = { "Int" };
	char* typeString[] = { "String" };
	char* whileAnnounce[] = { "WHILE"};
	char* startCondition[] = {"If"};
	char* secondaryCondition[] = { "Elif" };
	char* finalConditon[] = { "Else" };
	char* modifiers[] = { "=","+=","-=","*=","/=" };
	char* operators[] = { "+","-","*","/","%","==","!=",">=","<=",">","<"};
	int operatorsPrecedence[] = { 1,1,2,2,2,0,0,0,0,0,0 };
	char* openParenthesis = "(";
	char* closedParenthesis = ")";
	char* openIndex = "[";
	char* closeIndex = "]";
	char* statementStart = "{";
	char* statementEnd = "}";
	char* builtInFunction = "PRINT";
	char* endOfFile = "\0";
	char* stringLiteral = "\"";
	char* charLiteral = "\'";
	char* separator = " ";
	char* newLine = "\n";
	char* commandAdd = ",";
	char f = EOF;
	int i,mode = 0;
	for (i = 0; i < sizeof(typeString) / sizeof(typeString[0]); i++) {
		addWord(typeString[i], (int)strlen(typeString[i]), createTokenId(TYPE_STRING_DECLARE, typeString[i], 0), mode++, 0);
	}
	for (i = 0; i < sizeof(startCondition) / sizeof(startCondition[0]); i++) {
		addWord(startCondition[i], (int)strlen(startCondition[i]), createTokenId(PRIMARY_CONDITION, startCondition[i], 0), mode++, 0);
	}
	for (i = 0; i < sizeof(secondaryCondition) / sizeof(secondaryCondition[0]); i++) {
		addWord(secondaryCondition[i], (int)strlen(secondaryCondition[i]), createTokenId(SECONDARY_CONDITION, secondaryCondition[i], 0), mode++, 0);
	}
	for (i = 0; i < sizeof(finalConditon) / sizeof(finalConditon[0]); i++) {
		addWord(finalConditon[i], (int)strlen(finalConditon[i]), createTokenId(FINAL_CONDITION, finalConditon[i], 0), mode++, 0);
	}
	for (i = 0; i < sizeof(typeInt) / sizeof(typeInt[0]); i++) {
		addWord(typeInt[i], (int)strlen(typeInt[i]), createTokenId(TYPE_INTEGER_DECLARE, typeInt[i], 0),mode++,0);
	}
	

	for (i = 0; i < sizeof(whileAnnounce) / sizeof(whileAnnounce[0]); i++) {
		addWord(whileAnnounce[i], (int)strlen(whileAnnounce[i]), createTokenId(WHILE_STATEMENT, whileAnnounce[i], 0), mode++,0);
	}
	for (i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
		addWord(operators[i], (int)strlen(operators[i]), createTokenId(OPERATOR, operators[i], operatorsPrecedence[i]), mode++, 1);
	}
	for (i = 0; i < sizeof(modifiers) / sizeof(modifiers[0]); i++) {
		addWord(modifiers[i], (int)strlen(modifiers[i]), createTokenId(MODIFIER, modifiers[i], 0), mode++,1);
	}
	
	addWord(openIndex, (int)strlen(openIndex), createTokenId(OPEN_PARENTHESES, openIndex, 0), mode++,1);
	addWord(closeIndex, (int)strlen(closeIndex), createTokenId(CLOSED_PARENTHESESE, closeIndex, 0), mode++,1);
	addWord(openParenthesis, (int)strlen(openParenthesis), createTokenId(OPEN_PARENTHESES, openParenthesis, 10), mode++,1);
	addWord(closedParenthesis, (int)strlen(closedParenthesis), createTokenId(CLOSED_PARENTHESESE, closedParenthesis, 10), mode++,1);
	addWord(statementStart, (int)strlen(statementStart), createTokenId(STATEMENT_START, statementStart, 0), mode++,1);
	addWord(statementEnd, (int)strlen(statementEnd), createTokenId(STATEMENT_END, statementEnd, 0), mode++,1);
	addWord(builtInFunction, (int)strlen(builtInFunction), createTokenId(BUILT_IN_FUNCTION, builtInFunction, 0), mode++,0);
	addWord(endOfFile, (int)strlen(endOfFile), createTokenId(END_OF_FILE, endOfFile, 0), mode++,0);
	addWord(stringLiteral, (int)strlen(stringLiteral), createTokenId(STRING_LITERAL, stringLiteral, 0), mode++,0);
	addWord(charLiteral, (int)strlen(charLiteral), createTokenId(CHAR_LITERAL, charLiteral, 0), mode++,0);
	addWord(separator, (int)strlen(separator), createTokenId(SEPARATOR, separator, 0), mode++,1);
	addWord(newLine, (int)strlen(newLine), createTokenId(END_OF_LINE, newLine, 0), mode++, 1);
	addWord(commandAdd, (int)strlen(commandAdd), createTokenId(COMMAND_ADD, commandAdd, 0), mode++, 1);
}
