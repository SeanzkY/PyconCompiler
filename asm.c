#define _CRT_SECURE_NO_WARNINGS
#include "asm.h"
#include "parser.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "Stack.h"

FILE* asmFile;
char* dataSeg = NULL;
char* codeSeg = NULL;
int stackVars = 0;
char* labelName = "label";
int labelCounter = 0;
Stack* numberPushed = NULL;
Stack* pointerValues = NULL;

void identifyAsm(parseTree** result);

void saveToDataSeg(char* command);

void saveToCodeSeg(char* command);

// get the next label for jump
char* nextLabel() {
    int size_needed = snprintf(NULL, 0, "%s%d", labelName,labelCounter);
    char* res = (char*)malloc(size_needed+1);
    sprintf(res, "%s%d", labelName, labelCounter);
    labelCounter++;
    return res;
}
// handle variables
char* handleVars(TokenId* var) {
    if (var->tok == IDENTIFIER && strcmp(symbolGet(var->id, var->scope)->scope, "0") != 0) {
        return symbolGet(var->id, var->scope)->codeName;
    }
    else {
        return var->id;
    }
}

// just for comparasion 
void compareSignAssembly(parseTree* t,char* command) {
    char* label, *label1;
    if (t->brother->son->brother == NULL) {
          saveToCodeSeg("\n    sub     ax, ");
          saveToCodeSeg(handleVars(t->brother->son->val));
    }
    else {
        saveToCodeSeg("\n    push ax ");
        computeExpression(&(t->brother));
        saveToCodeSeg("\n    pop bx ");
        saveToCodeSeg("\n    sub   ax,bx\n ");
        saveToCodeSeg("\n   neg ax\n ");
    }
    saveToCodeSeg("\n    cmp     ax,0\n ");
    saveToCodeSeg(command);
    label = nextLabel();
    saveToCodeSeg(label);
    saveToCodeSeg("\n    mov     ax,1 ");
    saveToCodeSeg("\n    jmp ");
    label1 = nextLabel();
    saveToCodeSeg(label1);
    saveToCodeSeg("\n");
    saveToCodeSeg(label);
    saveToCodeSeg(":");
    saveToCodeSeg("\n    mov     ax,0\n");
    saveToCodeSeg(label1);
    saveToCodeSeg(":");
}
// computing an expression via assembly
void computeExpression(parseTree** start) {
    parseTree* t = *start;
    char* label;
    char* label1;
    
    if (t->son->val->tok != -1) {
        saveToCodeSeg("\n    mov     ax, ");
        saveToCodeSeg(handleVars(t->son->val));

    }
    else
        computeExpression(&(t->son));
    t = t->son->brother;
    if (t) {
        if (strcmp(t->val->id, "+") == 0) {
            if (t->brother->son->brother == NULL) {
                 saveToCodeSeg("\n    add     ax, ");
                saveToCodeSeg(handleVars(t->brother->son->val));
            }
            else {
                saveToCodeSeg("\n   push ax ");
                computeExpression(&(t->brother));
                saveToCodeSeg("\n    pop     bx ");
                saveToCodeSeg("\n    add     ax,bx ");
            }
        }
        else if (strcmp(t->val->id, "-") == 0) {
            if (t->brother->son->brother == NULL) {
                saveToCodeSeg("\n    sub     ax, ");
                saveToCodeSeg(handleVars(t->brother->son->val));
            }
            else {
                saveToCodeSeg("\n   push ax ");
                computeExpression(&(t->brother));
                saveToCodeSeg("\n    pop     bx ");
                saveToCodeSeg("\n    sub     ax,bx ");
                saveToCodeSeg("\n   neg ax ");
            }
           
          

        }
        else if (strcmp(t->val->id, "*") == 0) {
            if (t->brother->son->brother == NULL) {
                saveToCodeSeg("\n    mov     bx, ");
                saveToCodeSeg(handleVars(t->brother->son->val));
                saveToCodeSeg("\n    mul     bx ");
            }
            else {
                saveToCodeSeg("\n   push ax ");
                computeExpression(&(t->brother));
                saveToCodeSeg("\n    pop     bx ");
                saveToCodeSeg("\n    mul bx ");
            }
        }
        else if (strcmp(t->val->id, "/") == 0) {
            if (t->brother->son->brother == NULL) {
                  saveToCodeSeg("\n    mov     bx, ");
                 saveToCodeSeg(handleVars(t->brother->son->val));
                 saveToCodeSeg("\n    xor     dx,dx");
                 saveToCodeSeg("\n    div     bx ");
            }
            else {
                saveToCodeSeg("\n   push ax ");
                computeExpression(&(t->brother));
                saveToCodeSeg("\n    pop     bx ");
                saveToCodeSeg("\n    xor     dx,dx");
                saveToCodeSeg("\n   xchg ax,bx ");
                saveToCodeSeg("\n    div bx ");
            }
        }
        else if (strcmp(t->val->id, "%") == 0) {
            if (t->brother->son->brother == NULL) {
                saveToCodeSeg("\n    mov     bx, ");
                saveToCodeSeg(handleVars(t->brother->son->val));
                saveToCodeSeg("\n    xor     dx,dx");
                saveToCodeSeg("\n    div     bx ");
                saveToCodeSeg("\n    mov     ax,dx");
            }
            else {
                saveToCodeSeg("\n   push ax ");
                computeExpression(&(t->brother));
                saveToCodeSeg("\n    pop     bx ");
                saveToCodeSeg("\n    xor     dx,dx");
                saveToCodeSeg("\n   xchg ax,bx ");
                saveToCodeSeg("\n    div bx ");
                saveToCodeSeg("\n    mov     ax,dx");
            }
        }
        else if (strcmp(t->val->id, "==") == 0) {
            compareSignAssembly(t, "jne ");
        }
        else if (strcmp(t->val->id, ">=") == 0) {
            compareSignAssembly(t, "jl ");
        }
        else if (strcmp(t->val->id, "<=") == 0) {
            compareSignAssembly(t, "jg ");
        }
        else if (strcmp(t->val->id, ">") == 0) {
            compareSignAssembly(t, "jle ");
        }
        else if (strcmp(t->val->id, "<") == 0) {
            compareSignAssembly(t, "jge ");
        }
        else if (strcmp(t->val->id, "!=") == 0) {
            compareSignAssembly(t, "je ");
        }
    }
}

// save data to data seg to write it later
void saveToDataSeg(char* command) {
    static int dataLen = 1;
    static int dataMax = 1;
    dataLen += (int)strlen(command);
    while (dataMax < dataLen) {
        dataMax += 1;
        dataMax *= 2;
    }
    dataSeg = (char*)realloc(dataSeg, sizeof(char) * dataMax);
    strcat(dataSeg, command);
}

// save data to code seg to write it later
void saveToCodeSeg(char* command) {
    static int dataLen = 1;
    static int dataMax = 1;
    dataLen += (int)strlen(command);
    while (dataMax < dataLen) {
        dataMax += 1;
        dataMax *= 2;
    }
    codeSeg = (char*)realloc(codeSeg, sizeof(char) * dataMax);
    strcat(codeSeg, command);
}

// to access local variables in asm
char* getStackLocation() {
    stackVars++;
    int size_needed = snprintf(NULL, 0, "[bp-%d]", stackVars*2);
    char* res = (char*)malloc(size_needed+1);
    sprintf(res, "[bp-%d]", stackVars * 2);
    return res;
}

void copySubStr(char* str) {
    int i;
    int adjust = -2;
    char* tempStr = (char*)malloc(sizeof(char) * 4);
    if (!tempStr)
        return NULL;
    tempStr[3] = '\0';
    tempStr[0] = '\'';
    tempStr[2] = '\'';
    if (strcmp(str, "\n") == 0) {
        saveToCodeSeg("\n   mov [bx],");
        saveToCodeSeg("10");
        saveToCodeSeg("\n   inc bx");
        saveToCodeSeg("\n   mov [bx],");
        saveToCodeSeg("13");
        saveToCodeSeg("\n   inc bx");
    }
    else {
        if (!strchr(str, '\"'))
            adjust = 0;
        for (i = 0; i < strlen(str) + adjust; i++) {
            saveToCodeSeg("\n   mov [bx],");
            tempStr[1] = str[i + 1];
            saveToCodeSeg(tempStr);
            saveToCodeSeg("\n   inc bx");
        }
        saveToCodeSeg("\n   mov [bx],0");
    }
   
}

void copyFromStrToStr(TokenId* first,TokenId* second) {
    char* startLabel = nextLabel();
    saveToCodeSeg("\n   mov si,ds");
    saveToCodeSeg("\n   mov es,si");
    saveToCodeSeg("\n   mov si,");
    saveToCodeSeg(handleVars(second));
    saveToCodeSeg("\n   mov di,");
    saveToCodeSeg(handleVars(first));
    saveToCodeSeg("\n");
    saveToCodeSeg(startLabel);
    saveToCodeSeg(":");
    saveToCodeSeg("\n  lodsb");
    saveToCodeSeg("\n  stosb");
    saveToCodeSeg("\n  cmp al,0");
    saveToCodeSeg("\n  jne ");
    saveToCodeSeg(startLabel);
    saveToCodeSeg("\n");
   

}

void calculateStrLen(TokenId* id) {
    saveToCodeSeg("\nmov cx,ds");
    saveToCodeSeg("\nmov es,cx");
    saveToCodeSeg("\nmov cx,-1");
    saveToCodeSeg("\nmov di,");
    saveToCodeSeg(handleVars(id));
    saveToCodeSeg("\nmov al,0");
    saveToCodeSeg("\nrepne scasb");
    saveToCodeSeg("\nmov bx,-1");
    saveToCodeSeg("\nsub bx,cx");
    saveToCodeSeg("\ndec bx");
}

void allocateStrMemory(TokenId* ident) {
    char* len;
    int size_needed;
    char* temp;
    int adjust = -1;
    if (ident->tok != IDENTIFIER) {
        len = ident->id;
        size_needed = snprintf(NULL, 0, "%d", (int)strlen(len));
        temp = (char*)malloc(size_needed + 1);
        if (!strchr(len, '\"'))
            adjust = 1;
        sprintf(temp, "%d", (int)strlen(len) + adjust);
        saveToCodeSeg("\nmov bx,");
        saveToCodeSeg(temp);
    }
    else {
        calculateStrLen(ident);
    }
    saveToCodeSeg("\nmov ah, 48h\n");
    saveToCodeSeg("\nint 21h\n");
}

void rewriteStr(TokenId* first, TokenId* second) {
    char* temp;
    char* len = "";
    int size_needed = -1;
    saveToCodeSeg("\nmov bx, es: [2]\n");
    saveToCodeSeg("int 21h\n");
    allocateStrMemory(second);
}
void handleAssigment(parseTree* id) {
    int i;
    int* addNumStack;
    char* temp;
    char* nameUsed = "";
    int size_needed;
    if (symbolGet(id->val->id, id->val->scope)->type == TYPE_INTEGER_DECLARE) {
        if (strcmp(symbolGet(id->val->id, id->val->scope)->scope, "0") == 0) {
            computeExpression(&(id->brother->brother));
            saveToCodeSeg("\n     mov   ");
            saveToCodeSeg(id->val->id);
            saveToCodeSeg(",ax");
        }
        else {
            computeExpression(&(id->brother->brother));
            saveToCodeSeg("\n     mov   ");
            saveToCodeSeg(symbolGet(id->val->id, id->val->scope)->codeName);
            saveToCodeSeg(",ax");
        }
    }
    else {
        nameUsed = handleVars(id->val);
        saveToCodeSeg("\nmov ah, 49h\n");
        saveToCodeSeg("mov es, ");
        saveToCodeSeg(nameUsed);
        saveToCodeSeg("\n");
        rewriteStr(id->val, id->brother->brother->val);
        saveToCodeSeg("\n     mov   ");
        saveToCodeSeg(nameUsed);
        saveToCodeSeg(",ax\n");
        saveToCodeSeg("mov bx,ax\n");
        if (id->brother->brother->val->tok != IDENTIFIER)
            copySubStr(id->brother->brother->val->id);
        else
            copyFromStrToStr(id->val, id->brother->brother->val);
    
    }
}


// for declaration in assembly
void declarationASM(parseTree** result) {
    parseTree* t = *result;
    IdentiferType* temp;
    int i;
    int* addNumStack;
    int* garbadgeCollect = (int*)calloc(sizeof(int), 1);
     char* tempStr;
    int size_needed;
    if (asmFile == NULL) {
        printf("Error opening file!\n");
        return;
    }
    if (t->son->val->tok == TYPE_INTEGER_DECLARE) {
        if (strcmp(symbolGet(t->son->brother->val->id, t->son->brother->val->scope)->scope, "0") == 0) {
            fprintf(asmFile, "    %s     dw ?\n",t->son->brother->val->id);
            computeExpression(&(t->son->brother->brother->brother));
            saveToCodeSeg("\n     mov   ");
            saveToCodeSeg(t->son->brother->val->id);
            saveToCodeSeg(",ax");
        }
        else {
            computeExpression(&(t->son->brother->brother->brother));
            saveToCodeSeg(" \n   push ax\n");
            symbolGet(t->son->brother->val->id, t->son->brother->val->scope)->codeName = getStackLocation();
            addNumStack = (int*)pop(numberPushed);
            (*addNumStack)++;
            push(numberPushed,addNumStack);
            push(pointerValues, garbadgeCollect);
            
        }    
    }
    else { 
        if (strcmp(symbolGet(t->son->brother->val->id, t->son->brother->val->scope)->scope, "0") == 0) {
            fprintf(asmFile, "    %s     dw ?\n", t->son->brother->val->id);
            temp = symbolGet(t->son->brother->val->id, t->son->brother->val->scope);
            allocateStrMemory(t->son->brother->brother->brother->val);
            saveToCodeSeg("mov ");
            saveToCodeSeg(t->son->brother->val->id);
            saveToCodeSeg(",ax\n ");
            saveToCodeSeg("mov bx,ax");
            if (t->son->brother->brother->brother->val->tok != IDENTIFIER)
                copySubStr(t->son->brother->brother->brother->val->id);
            else
                copyFromStrToStr(t->son->brother->val, t->son->brother->brother->brother->val);
            

        }
        else {
            *garbadgeCollect = 1;
            allocateStrMemory(t->son->brother->brother->brother->val);
            saveToCodeSeg("push ax\n ");
            saveToCodeSeg("mov bx,ax");
            temp = symbolGet(t->son->brother->val->id, t->son->brother->val->scope);
            temp->codeName = getStackLocation();
            if (t->son->brother->brother->brother->val->tok != IDENTIFIER)
                copySubStr(t->son->brother->brother->brother->val->id);
            else
                copyFromStrToStr(t->son->brother->val, t->son->brother->brother->brother->val);
            addNumStack = (int*)pop(numberPushed);
            (*addNumStack)++;
            push(numberPushed, addNumStack);
            push(pointerValues, garbadgeCollect);
           

        }
    }
    *result = t->brother;
    
}
void conditionASM(parseTree** result,char* lastLabel) {
    int isFirst = lastLabel == NULL;
    parseTree* t = *result;
    if (!t)
        return;
    parseTree* temp;
    char* label = nextLabel();
    if (t && strcmp(t->val->id, "Else") != 0) {
        computeExpression(&(t->brother));
        saveToCodeSeg("\n   cmp ax,0");
        saveToCodeSeg("\n   je ");
        saveToCodeSeg(label);
    }
    temp = t;
    identifyAsm(&(t->brother->brother));
    saveToCodeSeg("\njmp ");
    if (isFirst)
        lastLabel = nextLabel();
    saveToCodeSeg(lastLabel);
    saveToCodeSeg("\n");
    if (t && strcmp(t->val->id, "Else") != 0) {
        saveToCodeSeg(label);
        saveToCodeSeg(":");
    }
    if (t && t->brother && t->brother->brother) {
        conditionASM(&(t->brother->brother->brother), lastLabel);
        t = t->brother->brother->brother;
       
    }
    if (isFirst) {
        saveToCodeSeg("\n");
        saveToCodeSeg(lastLabel);
        saveToCodeSeg(":"); 

    }

}

void startDataSeg() {
    
  
    fprintf(asmFile, "  .STACK 100h\n");
    fprintf(asmFile, ".DATA \n");
    
}
void startCodeSeg() {
    fprintf(asmFile, ".CODE\n");
    fprintf(asmFile, codeSeg);
  
}
void startCode() {
    
    asmFile = fopen("assembly_code.asm", "w");
    if (asmFile == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
   
   
}

void endCode() {
    if (asmFile == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(asmFile, "    ret\n"); 
    fprintf(asmFile, "main ENDP\n");
    fprintf(asmFile, "END main\n");
}

void handleWhile(parseTree** result) {
    parseTree* t = *result;
    char* startLabel = nextLabel();
    char* lastLabel = nextLabel();
    saveToCodeSeg("\n");
    saveToCodeSeg(startLabel);
    saveToCodeSeg(":");
    computeExpression(&(t->brother));
    saveToCodeSeg("\n   cmp ax,0");
    saveToCodeSeg("\n   je ");
    saveToCodeSeg(lastLabel);
    saveToCodeSeg("\n");
    t = t->brother->brother;
    identifyAsm(&(t));
    saveToCodeSeg("\njmp ");
    saveToCodeSeg(startLabel);
    saveToCodeSeg("\n ");
    saveToCodeSeg(lastLabel);
    saveToCodeSeg(":");
    *result = t;
}

void printNumber() {
    char* startLabel = nextLabel();
    char* printLabel = nextLabel();
    char* notNegative = nextLabel();
    saveToCodeSeg("\n   cmp ax,0");
    saveToCodeSeg("\n   jge ");
    saveToCodeSeg(notNegative);
    saveToCodeSeg("\n   neg ax");
    saveToCodeSeg("\n  push ax\n");
    saveToCodeSeg("\n  mov dl, 45\n");
    saveToCodeSeg("\n  mov ah,2\n");
    saveToCodeSeg("\n  int 21h\n");
    saveToCodeSeg("\n  pop ax\n");
    saveToCodeSeg(notNegative);
    saveToCodeSeg(":\n");
    saveToCodeSeg("\n   xor cx,cx\n");
    saveToCodeSeg(startLabel);
    saveToCodeSeg(":");
    saveToCodeSeg("\n   xor dx,dx");
    saveToCodeSeg("\n   mov bx,10");
    saveToCodeSeg("\n   div bx");
    saveToCodeSeg("\n  add dl,30h");
    saveToCodeSeg("\npush dx");
    saveToCodeSeg("\n   inc cx");
    saveToCodeSeg("\n   cmp ax,0");
    saveToCodeSeg("\n   jne ");
    saveToCodeSeg(startLabel);
    saveToCodeSeg("\n");
    saveToCodeSeg(printLabel);
    saveToCodeSeg(":");
    saveToCodeSeg("\npop dx");
    saveToCodeSeg("\n  mov ah,2");
    saveToCodeSeg("\n  int 21h");
    saveToCodeSeg("\n  loop ");
    saveToCodeSeg(printLabel);
   
}

void handlePrint(parseTree** result) {
    parseTree* t = *result;
    int size_needed;
    char* res;
    char* label;
    TokenId* temp = t->brother->brother->val;
    if (temp->tok == IDENTIFIER && symbolGet(temp->id, temp->scope)->type == TYPE_INTEGER_DECLARE) {
        saveToCodeSeg("\nmov ax,");
        saveToCodeSeg(handleVars(temp));
        printNumber();
    }
    else if(temp->tok == IDENTIFIER){
        label  = nextLabel();
        saveToCodeSeg("\nmov ah,2");
        saveToCodeSeg("\nmov si,");
        saveToCodeSeg(handleVars(temp));
        calculateStrLen(temp);
        saveToCodeSeg("\nmov cx,bx");
        saveToCodeSeg("\n");
        saveToCodeSeg(label);
        saveToCodeSeg(":");
        saveToCodeSeg("\nlodsb");
        saveToCodeSeg("\nmov dl,al");
        saveToCodeSeg("\nmov ah,2");
        saveToCodeSeg("\nint 21h");
        saveToCodeSeg("\nloop ");
        saveToCodeSeg(label);
    }
}

void identifyAsm(parseTree** result) {
    parseTree* t = *result;
    parseTree* temp;
    int* helper;
    int* numberStart = calloc(sizeof(int),1);
    if (t) {
        if (strcmp(t->val->id, "declaration") == 0) {
            declarationASM(&t);
        }
        else if (strcmp(t->val->id, "condition") == 0) {
            conditionASM(&(t->son),NULL);
            t = t->brother;
        }
        else if (strcmp(t->val->id, "statement") == 0) {
            temp = t->son;
            push(numberPushed, numberStart);
            while (temp && temp->brother && temp->brother->val->tok != STATEMENT_END) {
                identifyAsm(&(temp->brother));
            }
            numberStart = pop(numberPushed);
            while (*numberStart) {
                helper = pop(pointerValues);
                if (*helper) {
                    saveToCodeSeg("\nmov ah, 49h\n");
                    saveToCodeSeg("pop es");
                    saveToCodeSeg("\nmov bx, es: [2]");
                    saveToCodeSeg("\nint 21h\n");
                }
                else {
                    saveToCodeSeg("\n   pop ax\n");
                }
                free(helper);
                stackVars--;
                (*numberStart)--;
            } 
        }
        else if (strcmp(t->val->id, "assignment") == 0) {
            handleAssigment(t->son);
            t = t->brother;
        }
        else if (strcmp(t->val->id, "while") == 0 ) {
            handleWhile(&(t->son));
            t = t->brother;
        }
        else if (strcmp(t->val->id, "print") == 0) {
            handlePrint(&(t->son));
            t = t->brother;
        }
    }
    *result = t;
}

void handleAsm(parseTree* t) {
    numberPushed =  stackCreate(sizeof(int*));
    pointerValues = stackCreate(sizeof(int*));
    dataSeg = malloc(sizeof(char));
    codeSeg = malloc(sizeof(char));
    dataSeg[0] = '\0';
    codeSeg[0] = '\0';
    saveToCodeSeg("\nmov ax, DATA");
    saveToCodeSeg("\nmov ds, ax");
    saveToCodeSeg("\nmov es, ax");
    saveToCodeSeg("\nmov bp, sp");
    startCode();
    startDataSeg();
    while (t && t->val->tok != END_OF_FILE) {
        identifyAsm(&t);
       
    }
    saveToCodeSeg("\n mov ax, 4c00h");
    saveToCodeSeg("\n int 21h");
    startCodeSeg();
    fclose(asmFile);
    printf("Assembly code has been written to assembly_code.asm\n");
    return;
}