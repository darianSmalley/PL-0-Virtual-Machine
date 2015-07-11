//
//  util.h
//  0 VM
//
//  Created by Darian Smalley on 7/1/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#ifndef ____VM__util__
#define ____VM__util__

#include <stdio.h>
#include <stdlib.h>

typedef struct Instruction{
    int op; /*opcode*/
    int l; //Lexographical Level
    int m; //modifier
    int line_number; //line number
} IR;

FILE* openFile(char* fileName);
char* fillInputStream(char* fileName);

#endif /* defined(____VM__util__) */
