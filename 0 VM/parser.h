//
//  parser.h
//  0 VM
//
//  Created by Darian Smalley on 6/29/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#ifndef ____VM__parser__
#define ____VM__parser__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "util.h"
#include "interpreter.h"

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
    int kind;
    char name[12];
    int val;
    int level;
    int addr;
} symbol;

//Utilities for parser operation
int startParser(int fileName);
void getToken();
void checkIdentSym();

/*Fucntion: 
 searchTable()=>searches gobal symbol table for symbol with the same name.
 Return:
    if sym found, it returns the index for that symbol
    if sym not found, return -1
 */
int searchTable(char* name);
bool isDefined(char* token);
void printSymbolTable();

//Parser Operation
void PROGRAM();
void BLOCK();
void STATEMENT();
void EXPRESSION();
void CONDITION();
void TERM();
void FACTOR();
#endif /* defined(____VM__parser__) */
