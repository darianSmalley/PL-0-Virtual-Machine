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

typedef enum {
    LIT = 1, OPR = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8, SIO = 9
} ISA;

typedef enum {
    OPR_RET = 0, OPR_NEG = 1, OPR_ADD = 2, OPR_SUB = 3, OPR_MUL = 4, OPR_DIV = 5, OPR_ODD = 6, OPR_MOD = 7, OPR_EQL = 8, OPR_NEQ = 9, OPR_LSS = 10, OPR_LEQ = 11, OPR_GTR = 12, OPR_GEQ = 13
} arith_opr;

static IR codeTable[MAX_CODE_LENGTH] = {0};
static int cx = 0;

int startParser (int printToConsole) {
    lexemeList = fillInputStream("lexemeList.txt");
    
    PROGRAM();
    //TODO: check print option to print intermediate code to console.
    return 0;
}

void emit(int op, int l, int m) {
    
    if( cx > MAX_CODE_LENGTH) {
        puts("error: code index excedded MAX_CODE_LENGTH");
        exit(1);
    }
    else {
        codeTable[cx].op = op;
        codeTable[cx].l = l;
        codeTable[cx].m = m;
        cx++;
    }
}

void PROGRAM() {
    getToken();
    BLOCK();
    if (TOKEN != periodsym) {
        puts("error 9: expected period symbol.");
        exit(1);
    }
    emit(SIO, 0, 2); //halt
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
            
            getToken();
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
            
            emit(INC, 0, 1);
            
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
        
        lvl--;
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
        
        //TODO: TEST
        
        getToken();
        
        EXPRESSION();
        
        emit(STO, lvl - sym.level, sym.addr);
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
        //TODO: emit unconditional jump to appropriate program counter or code index
        
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
        
        int tmp = cx;
        emit(JPC, 0, 0);
        
        STATEMENT();
        
        codeTable[tmp].m = cx;
    }
    else if (TOKEN == whilesym ) {
        int cx1 = cx;
        getToken();
        CONDITION();
        
        int cx2 = cx;
        emit(JPC, 0,0);
        
        if ( TOKEN != dosym) {
            puts("error 17: expected do symbol.");
            exit(1);
        }
        
        getToken();
        
        STATEMENT();
        emit(JMP, 0,cx1);
        codeTable[cx2].m = cx;
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
        //relevent val will be at the top of stack at this point
        
//        if (TOKEN != eqlsym  || TOKEN != neqsym || TOKEN != lessym || TOKEN != leqsym || TOKEN != gtrsym || TOKEN != geqsym) {
//            puts("error 20: expected relational operatior.");
//            exit(1);
//        }
        int relop = 0;
        
        switch (TOKEN) {
            case eqlsym:
                relop = OPR_EQL;
                break;
            case neqsym:
                relop = OPR_NEQ;
                break;
            case lessym:
                relop = OPR_LSS;
                break;
            case leqsym:
                relop = OPR_LEQ;
                break;
            case gtrsym:
                relop = OPR_GTR;
                break;
            case geqsym:
                relop = OPR_GEQ;
                break;
            default:
                puts("error 20: expected relational operatior.");
                exit(1);
        }
        
        getToken();
        
        EXPRESSION();
        
        //top two stack elements are relevent values for comparison
        //stack arithmetic operation emit here, if top of stack == 0, JPC will execute, otherwise it wont
        emit(OPR, 0, relop);
    }
}

void EXPRESSION() {
    int addop;
    
    if (TOKEN == plussym || TOKEN == minussym ) {
        addop = TOKEN;
        getToken();
        TERM();
        if(addop == minussym)
            emit(OPR, 0, OPR_NEG); //negate
    }
    else
        TERM();
    
    while (TOKEN == plussym || TOKEN == minussym ) {
        addop = TOKEN;
        getToken();
        TERM();
        if ( addop == plussym)
            emit(OPR, 0, OPR_ADD); //addition
        else
            emit(OPR, 0, OPR_SUB); //subtraction
    }
}

void TERM() {
    int mulop;
    FACTOR();
    
    while (TOKEN == multsym || TOKEN == slashsym) {
        mulop = TOKEN;
        getToken();
        FACTOR();
        if( mulop == multsym)
            emit(OPR, 0, OPR_MUL); //multiplication
        else
            emit(OPR, 0, OPR_DIV); //division
    }
}

void FACTOR() {
    if ( TOKEN == identsym ) {
        char* tokenName = strsep(&lexemeList, " ");
        checkIdentSym(tokenName);
        
        symbol sym = symbol_table[searchTable(tokenName)];
        
        if ( sym.kind == procsym) {
            puts("error 21: Expression must not contain a procedure identifier.");
            exit(1);
        }
        //load identifer value to top of stack
        if ( sym.kind == varsym)
            emit(LOD, lvl - sym.level, sym.addr);
        else if (sym.kind == constsym)
            emit(LIT, 0, sym.val);
        
       getToken();
    }
    else if ( TOKEN == numbersym) {
        getToken();
        //push number value to top of stack
        emit(LIT, 0, TOKEN);
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