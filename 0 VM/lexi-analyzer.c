//
//  lexi-analyzer.c
//  0 VM
//
//  Created by Darian Smalley on 6/3/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define norw 15 /* number of reserved words */
#define imax 32767 /* maximum integer value */
#define cmax 11 /* maximum number of chars for idents */
#define nestmax 5 /* maximum depth of block nesting */
#define strmax 256 /* maximum length of strings */

static const int MAX_NAME_TABLE_SIZE = 256;
static const int MAX_NUM_LEN = 5;
static int lineNum = 0;
static int error = 0;

typedef enum {
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6,  slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym =12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym =31,
    readsym = 32, elsesym = 33
} token_type;

typedef struct {
    int kind; /*const = 1, var = 2, proc = 3*/
    char *name; /*name up to 11*/
    int val; /*number (ASCII value)*/
    int level; /*L level*/
    int adr;   /*M address*/
} namerecord_t;
namerecord_t symbol_table[MAX_NAME_TABLE_SIZE];
int index_symbolTable = 0;

/*list of reserved keywords*/
char *word[] = {"null","begin","call","cosnt","do","else","end","if","odd","procedure","read","then","var","while","write"};

/*internal representation of reserved keywords*/
int wsym[] = {nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};

/*list of special symbols*/
int ssym[256] = {0};

void fillSsym() {
    ssym['+'] = plussym;
    ssym['-'] = minussym;
    ssym['*'] = multsym;
    ssym['/'] = slashsym;
    ssym['('] = lparentsym;
    ssym[')'] = rparentsym;
    ssym['='] = eqlsym;
    ssym[','] = commasym;
    ssym['.'] = periodsym;
    ssym['#'] = neqsym;
    ssym['<'] = lessym;
    ssym['>'] = gtrsym;
    ssym['$'] = leqsym;
    ssym['%'] = geqsym;
    ssym[';'] = semicolonsym;
    ssym[':'] = becomessym;
}

int isSpecialSym(char ch) {
    if ( ssym[ch] != 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int convert(char ch) {
    return (ch - '0');
}

int intValueOf(char *s) {
    int result = 0;
    
    while (*s != '\0') {
        result = 10 * result + convert(*s);
    }
    
    return result;
}

void processToken(int start,int end, char stream[] ) {
    if ( !isspace(stream[start])) {
        int isReserve = 0;
        int i = 0;
        int len = end - start;
        char *token = malloc((len + 1) * sizeof(char));
        namerecord_t *r = malloc(sizeof(namerecord_t));
        
        memcpy( token, &stream[start], len);
        token[len] = '\0';
        
        if ( isdigit(*token)) {
            
            while ( token[i] != '\0') {
                if ( isalpha(token[i])) {
                    error = 1;
                    fprintf(stderr, "Lexical Error on line %d: Variable (%s) does not start with letter", lineNum, token);
                    exit(1);
                }
                i++;
            }
            r->kind = numbersym;
            r->name = token;
            r->val = intValueOf(token);
            
            if ( r->val > imax) {
                error = 1;
                fprintf(stderr, "Lexiacl Error on line %d: Number (%s) is too long", lineNum, token);
                exit(1);
            }
            
            r->level = 0;
            r->adr = 0;
        }
        else if ( isalpha(*token) ) {
            if ( len > cmax) {
                error = 1;
                fprintf(stderr, "Lexical Error on line %d: Identifier (%s) too long", lineNum, token);
                exit(1);
            }
            
            for (i = 0; i < norw; i++) {
                if ( strcmp ( word[i], token ) == 0) {
                    r->kind = wsym[i];
                    r->name = token;
                    r->val = 0;
                    r->level = 0;
                    r->adr = 0;
                    isReserve = 1;
                    break;
                }
            }
            
            if ( !isReserve ) {
                r->kind = identsym;
                r->name = token;
                r->val = 0;
                r->level = 0;
                r->adr = 0;
            }
            
        }
        
        symbol_table[index_symbolTable] = *r;
        index_symbolTable++;
    }
    
}

int processSym(int j, char stream[]) {
    char sym =  stream[j];
    char nextSym = stream[j + 1];
    char* symbol = 0;
    namerecord_t *r = malloc(sizeof(namerecord_t));
    int two_character_sym = 0;

    if ( (sym == ':' && nextSym == '=') || (sym == '<' && nextSym == '>') ) {
        symbol = malloc( 3 * sizeof(char));
        symbol[0] = sym;
        symbol[1] = nextSym;
        symbol[2] = '\0';
        r->kind = (sym == ':') ? ssym[sym] : ssym['#'];
        r->name = symbol;
        r->val = 0;
        r->level = 0;
        r->adr = 0;
        
        two_character_sym = 1;
    }
    else if ( sym == ':' && nextSym != '=') {
        error = 1;
        fprintf(stderr, "Lexical Error on line %d: Invalid Symbol (%c%c) ", lineNum, sym, nextSym);
        exit(1);
    }
    else {
        symbol = malloc (2*sizeof(char));
        symbol[0] = sym;
        symbol[1] = '\0';
        r->kind = ssym[sym];
        r->name = symbol;
        r->val = 0;
        r->level = 0;
        r->adr = 0;
    }
 
    symbol_table[index_symbolTable] = *r;
    index_symbolTable++;
    return two_character_sym;
}

void tokenizeInput(char inputStream[]) {
    int i = 0; //token being index
    int j = 0; //token end index
    int ch = 0;
    
    /*tokens are seperated by either whitespace or special symbols and operators*/
    while ( (ch = inputStream[j]) != '\0') {
        if ( isspace(ch) != 0 ) {
            processToken(i,j,inputStream);
            j++;
            i = j;
            
            if ( ch == '\n') {
                lineNum++;
            }
        }
        else if ( isSpecialSym(ch) ) {
            processToken(i,j,inputStream);

            if ( processSym(j, inputStream) ) {
                //two chracter symbol
                j = j + 2;
                i = j;
            } else {
                j++;
                i = j;
            }

        }
        else {
            j++;
        }
    }
}

char* fillInputStream(FILE* file) {
    int c;
    int i = 0;
    int commentFlag = 0;
    char previous = 0;
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *inputStream = malloc(fsize + 1);
    
    while ((c = fgetc(file)) != EOF) {
        if ( i > 1) {
            previous = inputStream [i - 1];
        }
        
        if ( c == '*' && previous == '/' && commentFlag == 0) {
            commentFlag = 1;
            inputStream[ i - 1] = ' ';
        }
        
        if ( commentFlag ) {
            if ( c == '/' && previous == '*') {
                commentFlag = 0;
            }
            
            c = ' ';
        }
        
        inputStream[i] = c;
        i++;
    }
    
    inputStream[i] = '\0';
    
    if ( commentFlag ) {
        error = 1;
        fprintf(stderr, "Lexical Error: Invalid Symbol - comment is not closed");
    }
    
    return inputStream;
}

FILE* openFile(char* fileName) {
    FILE* file;
    
    if (!(file = fopen(fileName, "r" ))) {
        perror("input.txt");
        exit(1);
    }
    
    return file;
}

void printCleanInput(char* source) {
    FILE* out = fopen("cleaninput.txt", "w");
    fwrite(source, sizeof(char), strlen(source), out);
    fclose(out);
}

void printLexemeData() {
    FILE* lexemeTable = fopen("lexemeTable.txt", "w");
    FILE *lexemeList = fopen("lexemeList.txt", "w");
    namerecord_t r;
    int i = 0;
    
    fprintf(lexemeTable, "%-15s %-15s\n", "lexeme", "token type");
    
    for( i = 0; i < index_symbolTable; i++) {
        r = symbol_table[i];
        
        fprintf(lexemeTable, "%-15s %-15d\n", r.name, r.kind);
        
        if ( r.kind == 2)
            fprintf(lexemeList, "%d %s",r.kind, r.name);
        else
            fprintf(lexemeList, "%d",r.kind);
    }
    
    fclose(lexemeTable);
    fclose(lexemeList);
}

void freeTable() {
    
    int i = 0;
    while ( i < index_symbolTable ) {
        free(symbol_table[i].name);
    }
    
    free(symbol_table);
}

int main(int argc, const char * argv[]) {
    FILE *file = openFile("input.txt");
    
    fillSsym();
    
    char* inputStream = fillInputStream(file);
    
    printCleanInput(inputStream);

    fclose(file);
    
    tokenizeInput(inputStream);
    
    printLexemeData();

    freeTable();
}
