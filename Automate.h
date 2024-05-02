#pragma once


#define ASCI_TABLE_SIZE 128
#define UNNECESSARY_ASCII_VALS 1
#define MAX_MODES 40

#include "Tokens.h"

// represents the arrow in the vecror that signal where to go next
typedef struct {
	// signals next node
	int nextMode;
	// signals the current mode to know to not go to a different node
	int modeNum;
	// the token that the current way returuns
	TokenId* currToken;
}nextModePoint;

// represents the value itself in the vector
typedef struct {
	nextModePoint** next;
	// number of modes
	int nodesCount;
	// for special chars that wont require space to seperate: '+', '*','/','-','='.... and more
	int noSpaceChar;
}finiteAutomation;




// find the index of a mode inside the dynmaic arr - that represents the arrows (in average case would be O(1))
int findModeIndex(nextModePoint** lst, int mode, int size);

// passes value through the state machine - get the word the next letter the curr index the mod and previous mode index
// returns the token it represents
TokenId* retToken(char* word, int nextLetter, int index, int modeIndex, int prevModeIndex);


// returns the token itself - uses retToken, add more things to it 
TokenId* tokenize(char* word);


// add one word to the automation
void addWord(char* word, int size, TokenId* token,int mode,int noSpaceChar);

// get an empty 128 X  100 matrix that represents the automation and initalize it to 
// the necessary values
void createAutomation();