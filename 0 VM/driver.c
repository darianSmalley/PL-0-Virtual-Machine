//
//  driver.c
//  0 VM
//
//  Created by Darian Smalley on 7/1/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//
#include "driver.h"

static int printLexeme = 0,
    printAssembly = 0,
    printStackTrace = 0;

int main(int argc, const char * argv[]) {
    int i = 0;
    
    for( i = 0; i < argc; i++) {
        if( strcmp(argv[i],"-l") == 0) {
            printLexeme = 1;
        }
        else if( strcmp(argv[i],"-a") == 0) {
            printAssembly = 1;
        }
        else if( strcmp(argv[i],"-v") == 0) {
            printStackTrace = 1;
        }
    }
    
    if ( startScanner(printLexeme) == 1 ) {
        puts("Lexical Error detected. Exiting Complier...");
        exit(1);
    }
    
    if( startParser(printAssembly) == 1) {
        puts("Syntatic Error Detected. Exiting...");
        exit(1);
    }
    else {
        puts("No errors, program is syntactically correct.");
    }
    
    startVM();
}