//
//  parser.c
//  0 VM
//
//  Created by Darian Smalley on 6/29/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"

char* lexemeList = 0;
int TOKEN = 0;

typedef enum {
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6,  slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym =12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym =31,
    readsym = 32, elsesym = 33
} token_type;

int parse (char* fileName) {
    lexemeList = fillInputStream(fileName);
    
    PROGRAM();
    
    return 0;
}

void PROGRAM() {
    getToken();
    BLOCK();
    if (TOKEN != periodsym) {
        puts("error");
        exit(1);
    }
}

void BLOCK() {
    if( TOKEN == constsym) {
        while( TOKEN != commasym ) {
            //get next token
            getToken();
            
            if (TOKEN != identsym) {
                puts("error");
                exit(1);
            }
            
            getToken();
            
            if (TOKEN != eqlsym) {
                puts("error");
                exit(1);
            }
            
            getToken();
            
            if (TOKEN != numbersym) {
                puts("error");
                exit(1);
            }
        }
    }
    
    if (TOKEN == varsym) {
        while( TOKEN != commasym) {
            getToken();
            
            if( TOKEN != identsym ) {
                puts("error");
                exit(1);
            }
            
            getToken();
        }
        
        if (TOKEN != semicolonsym) {
            puts("error");
            exit(1);
        }
        
        getToken();
    }
    
    while (TOKEN == procsym) {
        getToken();
        
        if (TOKEN != identsym) {
            puts("error");
            exit(1);
        }
        
        getToken();
        
        if (TOKEN != semicolonsym) {
            puts("error");
            exit(1);
        }
        
        getToken();
        
        BLOCK();
        
        if (TOKEN != semicolonsym) {
            puts("error");
            exit(1);
        }
    }
    STATEMENT();
}

void STATEMENT() {
    if ( TOKEN == identsym) {
        getToken();
        
        if (TOKEN != becomessym) {
            puts("error");
            exit(1);
        }
        
        getToken();
        
        EXPRESSION();
    }
    else if (TOKEN == callsym) {
        getToken();
        
        if ( TOKEN != identsym) {
            puts("error");
            exit(1);
        }
        
        getToken();
    }
    else if (TOKEN == beginsym ) {
        getToken();
        
        STATEMENT();
        
        while (TOKEN == semicolonsym ) {
            getToken();
            STATEMENT();
        }
        
        if ( TOKEN != endsym) {
            puts("error");
            exit(1);
        }
        
        getToken();
    }
    else if ( TOKEN == ifsym ) {
        getToken();
        
        CONDITION();
        
        if (TOKEN != thensym ) {
            puts("error");
            exit(1);
        }
        
        getToken();
        
        STATEMENT();
    }
    else if (TOKEN == whilesym ) {
        getToken();
        
        if ( TOKEN != dosym) {
            puts("error");
            exit(1);
        }
        
        getToken();
        
        STATEMENT();
    }
}

void EXPRESSION() {
    
}

void CONDITION() {
    
}

void TERM() {
    
}

void FACTOR() {
    
}

FILE* openFile(char* fileName) {
    FILE* file;
    
    if (!(file = fopen(fileName, "r" ))) {
        perror("input.txt");
        exit(1);
    }
    
    return file;
}

char* fillInputStream(char* fileName) {
    FILE *file = openFile(fileName);
    int c;
    int i = 0;
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *inputStream = malloc(fsize + 1);
    
    while ((c = fgetc(file)) != EOF) {
        inputStream[i] = c;
        i++;
    }
    
    fclose(file);
    
    inputStream[i] = '\0';
    
    return inputStream;
}

void getToken() {
    char* input = strsep(&lexemeList, " ");
    TOKEN = atoi(input);
}