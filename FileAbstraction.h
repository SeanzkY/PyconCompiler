#pragma once

// get the next letter in the file and move to the next letter
char nextLetter();

// revert file to former position using the amount of letters to return
void revertFile(int size);

// init file
void initFile(char* fileAddr);
