//
//  test.c
//  0 VM
//
//  Created by Darian Smalley on 6/12/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#include <stdio.h>

typedef struct {
    int kind; /*const = 1, var = 2, proc = 3*/
    char *name; /*name up to 11*/
    int val; /*number (ASCII value)*/
    int level; /*L level*/
    int adr;   /*M address*/
} namerecord_t;
static namerecord_t symbol_table[10];
static int index_symbolTable = 0;

void doStuff() {
    char *sym = "+";
    char *sym2 = "-";
    
    namerecord_t r = { 2, sym, 0 /*convert digit to ascii value for this field*/, 0, 0 };
    namerecord_t l = { 3, sym2, 1 /*convert digit to ascii value for this field*/, 0, 0 };
    symbol_table[index_symbolTable++] = r;
    symbol_table[index_symbolTable] = l;
}
int main(int argc, const char * argv[]) {
    doStuff();
    
    printf("%d %s %d %s\n", symbol_table[0].kind, symbol_table[0].name, symbol_table[1].kind, symbol_table[1].name);
    
}