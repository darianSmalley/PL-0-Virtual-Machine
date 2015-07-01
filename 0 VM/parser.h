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

int startParser(char* fileName);
FILE* openFile(char* fileName);
char* fillInputStream(char* fileName);
void getToken();
void PROGRAM();
void BLOCK();
void STATEMENT();
void EXPRESSION();
void CONDITION();
void TERM();
void FACTOR();
#endif /* defined(____VM__parser__) */
