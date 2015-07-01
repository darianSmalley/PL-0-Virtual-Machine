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

int startParser (char* fileName) {
    lexemeList = fillInputStream(fileName);
    
    PROGRAM();
    
    return 0;
}

void PROGRAM() {
    getToken();
    BLOCK();
    if (TOKEN != periodsym) {
        puts("error 9: expected period symbol.");
        exit(1);
    }
}

void BLOCK() {
    if( TOKEN == constsym) {
        while( TOKEN != commasym ) {
            //get next token
            getToken();
            
            if (TOKEN != identsym) {
                puts("error 4: expected identifier symbol.");
                exit(1);
            }
            
            getToken();
            
            if (TOKEN != eqlsym) {
                puts("error 3: expected equals symbol.");
                exit(1);
            }
            
            getToken();
            
            if (TOKEN != numbersym) {
                puts("error 2: '=' equal sym must be followed by a number.");
                exit(1);
            }
        }
    }
    
    if (TOKEN == varsym) {
        while( TOKEN != commasym) {
            getToken();
            
            if( TOKEN != identsym ) {
                puts("error 4: expected identifier symbol.");
                exit(1);
            }
            
            getToken();
        }
        
        if (TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol.");
            exit(1);
        }
        
        getToken();
    }
    
    while (TOKEN == procsym) {
        getToken();
        
        if (TOKEN != identsym) {
            puts("error 4: expected identifier symbol.");
            exit(1);
        }
        
        getToken();
        
        if (TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol");
            exit(1);
        }
        
        getToken();
        
        BLOCK();
        
        if (TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol.");
            exit(1);
        }
    }
    STATEMENT();
}

void STATEMENT() {
    if ( TOKEN == identsym) {
        getToken();
        
        if (TOKEN != becomessym) {
            puts("error: expected become symbol.");
            exit(1);
        }
        
        getToken();
        
        EXPRESSION();
    }
    else if (TOKEN == callsym) {
        getToken();
        
        if ( TOKEN != identsym) {
            puts("error 14: call must be followed by an identifier.");
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
            puts("error: expected end symbol.");
            exit(1);
        }
        
        getToken();
    }
    else if ( TOKEN == ifsym ) {
        getToken();
        
        CONDITION();
        
        if (TOKEN != thensym ) {
            puts("error 16: expected then symbol.");
            exit(1);
        }
        
        getToken();
        
        STATEMENT();
    }
    else if (TOKEN == whilesym ) {
        getToken();
        
        if ( TOKEN != dosym) {
            puts("error 17: expected do symbol.");
            exit(1);
        }
        
        getToken();
        
        STATEMENT();
    }
}

void CONDITION() {
    if (TOKEN == oddsym) {
        getToken();
        EXPRESSION();
    }
    else {
        EXPRESSION();
        
        if (TOKEN != eqlsym  || TOKEN != nulsym /*<---- this is incorrect and needs to be fixed*/ || TOKEN != lessym || TOKEN != leqsym || TOKEN != gtrsym || TOKEN != geqsym) {
            puts("error 20: expected relational operatior.");
            exit(1);
        }
        
        getToken();
        
        EXPRESSION();
    }
}

void EXPRESSION() {
    if (TOKEN == plussym || TOKEN == minussym ) {
        getToken();
        TERM();
    }
    
    while (TOKEN == plussym || TOKEN == minussym ) {
        getToken();
        TERM();
    }
}

void TERM() {
    FACTOR();
    
    while (TOKEN == multsym || TOKEN == slashsym) {
        getToken();
        FACTOR();
    }
}

void FACTOR() {
    if ( TOKEN == identsym ) {
        getToken();
    }
    else if ( TOKEN == numbersym) {
        getToken();
    }
    else if (TOKEN == lparentsym) {
        getToken();
        EXPRESSION();
        
        if (TOKEN != rparentsym) {
            puts("error 22: expected rpartent symbol '}'.");
            exit(1);
        }
        getToken();
    }
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