#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "FileAbstraction.h"


FILE* readFile = NULL;


char nextLetter() {
	char letter = fgetc(readFile);
	return letter;
}

void revertFile(int size) {
	fseek(readFile, -size, SEEK_CUR);
}

void initFile(char* fileAddr) {
	readFile = fopen(fileAddr, "rt");
	if (!readFile)
	{
		printf("error reading file\n");
	}
}