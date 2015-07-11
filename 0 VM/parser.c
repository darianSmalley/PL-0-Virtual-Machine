//
//  parser.c
//  0 VM
#include "parser.h"

static char* lexemeList = 0;
static int TOKEN = 0;
static symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
static int tableSize = 0;
static int mod = 0;
static int lvl = 0;

typedef enum {
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6,  slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym =12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym =31,
    readsym = 32, elsesym = 33
} token_type;

int startParser (int printToConsole) {
    lexemeList = fillInputStream("lexemeList.txt");
    
    PROGRAM();
    //TODO: check print option to print intermediate code to console.
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
        do {
            symbol *sym = malloc(sizeof(symbol));
            sym->kind = constsym;
            
            //get next token
            getToken();
            
            if (TOKEN != identsym) {
                puts("error 4: expected identifier symbol.");
                exit(1);
            }
            
            //get next token as char* instead of int because it is a symbol name
            char* input = strsep(&lexemeList, " ");
            memcpy(sym->name, input, strlen(input)+1);
            
            getToken();
            
            if (TOKEN != eqlsym) {
                puts("error 3: expected equals '=' symbol.");
                exit(1);
            }
            
            getToken();
            
            if (TOKEN != numbersym) {
                puts("error 2: expected number symbol.");
                exit(1);
            }
            getToken();
            
            sym->val = TOKEN;
            symbol_table[tableSize] = *sym;
            tableSize++;
            
            addIRtoCode(01, 0, sym->val);
            
            getToken();
            
            mod++;
        } while( TOKEN == commasym );
        
        if ( TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol after const sym.");
            exit(1);
        }
        
        getToken();
    }
    
    if (TOKEN == varsym) {
        do {
            symbol *sym = malloc(sizeof(symbol));
            sym->kind = varsym;
            
            getToken();
            
            if( TOKEN != identsym ) {
                puts("error 4: expected identifier symbol.");
                exit(1);
            }
            
            char* input = strsep(&lexemeList, " ");
            memcpy(sym->name, input, strlen(input)+1);
            
            sym->level = lvl;
            sym->addr = mod;
            
            symbol_table[tableSize] = *sym;
            tableSize++;
            
            getToken();
            mod++;
        } while( TOKEN == commasym);
        
        if (TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol after var sym.");
            exit(1);
        }
        
        getToken();
    }
    
    while (TOKEN == procsym) {
        symbol *sym = malloc(sizeof(symbol));
        sym->kind = procsym;
        
        getToken();
        
        if (TOKEN != identsym) {
            puts("error 4: expected identifier symbol.");
            exit(1);
        }
        
        char* input = strsep(&lexemeList, " ");
        memcpy(sym->name, input, strlen(input)+1);
        
        getToken();
        
        if (TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol after proc sym");
            exit(1);
        }
        
        sym->level = lvl;
        sym->addr = mod;
        
        lvl++;
        mod = 0;
        
        getToken();
        
        BLOCK();
        
        if (TOKEN != semicolonsym) {
            puts("error 5: expected semicolon symbol after block.");
            exit(1);
        }
    }
    STATEMENT();
}

void STATEMENT() {
    if ( TOKEN == identsym) {
        char* tokenName = strsep(&lexemeList, " ");
        checkIdentSym(tokenName);
        
        symbol sym = symbol_table[searchTable(tokenName)];
        
        if( sym.kind == constsym || sym.kind == procsym ) {
            puts("error 12: assignment to constant or procedure is not allowed.");
            exit(1);
        }
        
        getToken();
        
        if (TOKEN != becomessym) {
            if( TOKEN == eqlsym) {
                puts("error 1: found equals '=', expected becomes ':='.");
                exit(1);
            }
            else {
                puts("error: expected becomes symbol.");
                exit(1);
            }
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
        
        char* tokenName = strsep(&lexemeList, " ");
        checkIdentSym(tokenName);
        int index = searchTable(tokenName);
        
        if ( symbol_table[index].kind == constsym || symbol_table[index].kind == varsym) {
            puts("error 15: call of a constant or variable is meaningless.");
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
            puts("error 10: semicolon between statements missing.");
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
    else if( TOKEN == readsym ) {
        getToken();
        
        if ( TOKEN != identsym) {
            puts("error 14: call must be followed by an identifier.");
            exit(1);
        }
        
        char* tokenName = strsep(&lexemeList, " ");
        checkIdentSym(tokenName);
        
        getToken();
    }
    else if (TOKEN == writesym ) {
        getToken();
        
        if ( TOKEN != identsym) {
            puts("error 14: call must be followed by an identifier.");
            exit(1);
        }
        
        char* tokenName = strsep(&lexemeList, " ");
        checkIdentSym(tokenName);
        
        getToken();
    }
}

void CONDITION() {
    if (TOKEN == oddsym) {
        getToken();
        EXPRESSION();
    }
    else {
        EXPRESSION();
        
        if (TOKEN != eqlsym  || TOKEN != neqsym || TOKEN != lessym || TOKEN != leqsym || TOKEN != gtrsym || TOKEN != geqsym) {
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
    
    TERM();
    
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
        char* tokenName = strsep(&lexemeList, " ");
        checkIdentSym(tokenName);
        
        if ( symbol_table[searchTable(tokenName)].kind == procsym) {
            puts("error 21: Expression must not contain a procedure identifier.");
            exit(1);
        }
        
        getToken();
    }
    else if ( TOKEN == numbersym) {
        getToken();
        //calling get token a second time to move past the actual number
        //this is where numbers with arithmatic will be manipulated
        //DONT FORGET
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

void getToken() {
    char* input = strsep(&lexemeList, " ");
    TOKEN = atoi(input);
}

int searchTable(char* name) {
    int i = 0;
    
    for( i = 0; i < tableSize; i++) {
        if ( strcmp (name, symbol_table[i].name) == 0 )
            return i;
    }
    
    return -1;
}

bool isDefined(char* token) {
    if( searchTable(token) == -1 ) {
        return false;
    }
    else
        return true;
}

void checkIdentSym(char* name) {
    
    if ( !isDefined(name) ) {
        printf("error 11: Undeclared identifier '%s'\n", name);
        exit(1);
    }
}

void printSymbolTable() {
    int i = 0;
    for ( i = 0; i < tableSize; i++) {
        printf("%s ", symbol_table[i].name);
    }
    puts("");
}
