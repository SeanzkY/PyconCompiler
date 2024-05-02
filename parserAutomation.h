#pragma once
#include "Stack.h"
#include "Tokens.h"
#include "parser.h"
#include "LinkedList.h"

#define numMode 100
#define maxModeLen 100



typedef parseTree* (*parseFunc)(void);


// represent the instruction of pushing to stack and moving to the next mdoe
typedef struct {
	// next mode - the first index in the matrix
	int nextMode;

	int nextModeIndex;
	//next mode id
	int nextModeId;

}nextModeConnection;


// represents a modeId that is inside this unit of the matrix
typedef struct {
	// the number on the mode - to seperate it from modes with identical value in identical index
	int modeId;
	// the list is type nextModeConnection* !!!! - represents connection
	Node* modeConnections;

	TokenId* nonTerminal;

}mode;

// a list of all modeIDs
typedef struct {
	//reprsentes one unit of the matrix - all the mode Identifier that are there
	mode** currMode;
	int modeLen;
}modeArr;

// this is the main struct of the PDA
typedef struct {
	modeArr* arr[numMode][maxModeLen];
}PDA;

// this is an helper to insert values to the PDA - contains one mode's list to where it connects and the mode itself
typedef struct {
	Token modeIndex;
	mode* currMode;

}modeRepresent;

void initPda();


// finds the mode - based on it's identifier number - (returns it's index in the current list)
int findPDAMode(mode** lst, int mode,int size);


void addRoute(modeRepresent** lst, int size);

nextModeConnection* createModeConnection(Token nextMode, int nextModeIndex, int nextModeId);

modeRepresent* createModeRepresent(Token currMode, nextModeConnection** modeInstructions,
	int sizeInstructions, int modeID, TokenId* nonTerminal);

void createPDA();


// for the correct mode - call all sub routes inside it only for the first index - to passThroughPDA
parseTree* callPda(Node** list);

