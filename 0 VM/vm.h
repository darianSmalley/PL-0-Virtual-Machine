//
//  vm.h
//  0 VM
//
//  Created by Darian Smalley on 7/23/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#ifndef __VM_vm_h
#define __VM_vm_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"

int base (int level,int b);
void opr00_RET ();
void opr01_NEG ();
void opr02_ADD();
void opr03_SUB();
void opr04_MUL();
void opr05_DIV();
void opr06_ODD();
void opr07_MOD();
void opr08_EQL();
void opr09_NEQ();
void opr10_LSS();
void opr11_LEQ();
void opr12_GTR();
void opr13_GEQ();

void (*opr_fp[14]) ();

void op01_LIT (int l, int m);
void op02_OPR (int l, int m);
void op03_LOD (int l, int m);
void op04_STO (int l, int m);
void op05_CAL (int l, int m);
void op06_INC (int l, int m);
void op07_JMP (int l, int m);
void op08_JPC (int l, int m);
void op09_SIO (int l, int m);

void (*op_fp[9]) (int lexi, int mod);

void generateInst(IR *inst, char *terms[], int lineNumber);
void fillCode (char* fileName);
void printCode(FILE *file);
void printStack_Str(FILE *file);
void printInstructionTrace(FILE *file);
int isDivider(int i);
void matchStacks();
void removeTopAR();
void updateStack_str();

void fetch();
void execute();

int startVM();
#endif
