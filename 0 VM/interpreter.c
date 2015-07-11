#include "interpreter.h"

static IR codeTable[MAX_CODE_LENGTH] = {0};
static int codeSize = 0;

void addIRtoCode(int op, int l, int m) {
    IR *ir = malloc( sizeof(IR));
    ir->op = op;
    ir->l = l;
    ir->m = m;
    
    codeTable[codeSize] = *ir;
    codeSize++;
}

void printTable(int option){
    FILE* file = openFile("mcode.txt");
    
    //TODO: print table here, if option, print to console too
}