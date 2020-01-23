#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data.h"

typedef struct{
    int kind; 		// null = 1, identifier = 2
	char name[12];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
}symbol;

extern symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
tokenStruct tokenTable[MAX_SYMBOL_TABLE_SIZE];
int cx = 0, TOKEN, num, kind, lexlistx = 0, ErrCount = 0, diff, previousdiff = 0, regptr = 0;
char id[12];
int testCount = 0;

void parse();
void PROGRAM(symbol *table, instructions *code);
void GETTOKEN();
void BLOCK(int lexLevel, int tableIndex, symbol *table, instructions *code);
void CONSTANTS(int lexLevel, int *ptrx, int *pdx, symbol *table);
void VARIABLES(int lexLevel, int *ptrx, int *pdx, symbol *table);
void STATEMENT(int lexLevel, int *ptrx, instructions *code, symbol *table);
void CONDITION(int lexLevel, int *ptrx, instructions *code, symbol *table);
void EXPRESSION(int lexLevel, int *ptrx, instructions *code, symbol* table);
void TERM(int lexLevel, int *ptrx, instructions *code, symbol* table);
void FACTOR(int lexLevel, int *ptrx, symbol* table, instructions *code);
void emit(int op, int r, int l, int m, instructions *code);
void ENTER(int k, int *ptrx, int *pdx, int lexLevel, symbol* table);
void ERROR(int errorNum);
int POS(char *id, int *ptrx, symbol* table, int lexLevel);

void parse()
{
    FILE* ifp;
    FILE* ofp;

    ifp = fopen("scanOut.txt", "r");
    ofp = fopen("parseOut.txt", "w");

    int i;
    int lexLevel = 0; //Lexi level
    int data = 0; //data index
    int tableIndex = 0; //table index

    symbol table[MAX_SYMBOL_TABLE_SIZE] = {0};
    instructions code[MAX_CODE_LENGTH];

    PROGRAM(table, code);
	
	//code[0].m = 0;
	for(i = 0; i < cx; i++){
        fprintf(ofp, "%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
	    //printf("%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
	}
    fclose(ifp);
    fclose(ofp);
}


void PROGRAM(symbol *table, instructions *code){
	
    GETTOKEN();
    BLOCK(0, 0, table, code);

    if(TOKEN != 19)
        ERROR(9);
	
	//HALT
    emit(9, 0, 0, 3, code);
}

void GETTOKEN(){
	
    TOKEN = (int)tokenTable[lexlistx++].tokenID;

	//printf("%d ", TOKEN);

    if(TOKEN == 2)
    {
        lexlistx--;
        strcpy(id, tokenTable[lexlistx].name);
        //printf("\nthis is the string %s, %s\n", tokenTable[lexlistx].name, id);
        lexlistx++;
    }
    else if(TOKEN == 3)
    {
        lexlistx--;
        num = tokenTable[lexlistx].numberValue;
        lexlistx++;
    }

   // printf("%d ", TOKEN);
    lexListIndex++;
}


void BLOCK(int lexLevel, int tableIndex, symbol *table, instructions *code){

    if(lexLevel > MAX_LEXI_LEVELS)
        ERROR(26);

    int dx, table0, cx0;
    table0 = tableIndex;
    dx = 4;

    table[table0].addr = cx;
	
	//JMP
	emit(7, 0, 0, 0, code);

    do{
        if(TOKEN == 28){
            GETTOKEN();
            do{
                CONSTANTS(lexLevel, &tableIndex, &dx, table);
                while(TOKEN == 17){
                    GETTOKEN();
                    CONSTANTS(lexLevel, &tableIndex, &dx, table);
                }
                if(TOKEN == 18)
                    GETTOKEN();
                else
                    ERROR(5);

            } while(TOKEN == 2);
        }

        if(TOKEN == 29){
            GETTOKEN();
            do{

                VARIABLES(lexLevel, &tableIndex, &dx, table);
                while(TOKEN == 17){
                    GETTOKEN();
                    VARIABLES(lexLevel, &tableIndex, &dx, table);
                }

                if(TOKEN == 18)
                    GETTOKEN();
                else
                    ERROR(5);

            }while(TOKEN == 2);
        }

		while(TOKEN == 30){
            GETTOKEN();
            if(TOKEN == 2){
                ENTER(3, &tableIndex, &dx, lexLevel, table);
                GETTOKEN();
            }
            else
                ERROR(4);

            if(TOKEN == 18)
                GETTOKEN();

            else
                ERROR(5);

            BLOCK(lexLevel+1, tableIndex, table, code);
            if(TOKEN == 18){
				emit(2, 0, 0, 0, code);
				GETTOKEN();
			}
                
            else
                ERROR(5);
        }

    }while(TOKEN == 28 || TOKEN == 29 || TOKEN == 30);

    code[table[table0].addr].m = cx;
    table[table0].addr = cx;
    cx0 = cx;
	//INC
    emit(6, 0, 0, dx, code);
    STATEMENT(lexLevel, &tableIndex, code, table);
}

void CONSTANTS(int lexLevel, int *ptrx, int *pdx, symbol *table){
    if(TOKEN == 2){

        GETTOKEN();

        if((TOKEN == 9) || (TOKEN == 20)){

            GETTOKEN();

            if(TOKEN == 20)
                ERROR(1);

            if(TOKEN == 3){
                ENTER(1, ptrx, pdx, lexLevel, table);
                GETTOKEN();
            }
        }
    }
}

void VARIABLES(int lexLevel, int *ptrx, int *pdx, symbol *table){

    if(TOKEN == 2){
        ENTER(2, ptrx, pdx, lexLevel, table); //variable case

        GETTOKEN();
    }

    else
        ERROR(4);
}

void STATEMENT(int lexLevel, int *ptrx, instructions *code, symbol *table){

    int i, cx1, cx2;

    if(TOKEN == 2){
        i = POS(id, ptrx, table, lexLevel);
    //printf("this is i %d\n", i);
        if(i == 0)
            ERROR(11);
        else if(table[i].kind != 2){
            ERROR(12);
            i = 0;
        }
        GETTOKEN();
        if(TOKEN == 20)
            GETTOKEN();
        else
            ERROR(13);
        EXPRESSION(lexLevel, ptrx, code, table);
        if(i != 0){
			//STO
            emit(4, regptr, lexLevel - table[i].level, table[i].addr, code);
			regptr--;
			
			if(regptr < 0 )
				regptr = 0;
		}
    }

    else if(TOKEN == 27){
        GETTOKEN();
        if(TOKEN != 2)
            ERROR(14);
        else{
			
			emit(8, 0, 0, table[i].addr, code);
            i = POS(id, ptrx, table, lexLevel);
            if(i == 0)
                ERROR(11);
            else if(table[i].kind == 3){
				//CAL
                emit(5, 0, lexLevel, table[i].addr -1, code);
            }
            else
                ERROR(15);
        }
        GETTOKEN();
    }

    else if(TOKEN == 23){
        GETTOKEN();
        CONDITION(lexLevel, ptrx, code, table);
        if(TOKEN == 24){
            GETTOKEN();
            STATEMENT(lexLevel, ptrx, code, table);
        }
        else
            ERROR(16);

        cx1 = cx;

        if(TOKEN == 33){
            GETTOKEN();
            code[cx1].m = cx + 1;
            cx1 = cx;
			//JPC
            emit(8, regptr, 0, 0, code); //could be table.addr
            STATEMENT(lexLevel, ptrx, code, table);
        }
        code[cx1].m = cx;
    }

    else if(TOKEN == 21){
        GETTOKEN();
        STATEMENT(lexLevel, ptrx, code, table);
        while(TOKEN == 18){
            GETTOKEN();
            STATEMENT(lexLevel, ptrx, code, table);
        }
        if(TOKEN == 22)
            GETTOKEN();
        else
            ERROR(17);
    }

    else if(TOKEN == 25){
        cx1 = cx;
        GETTOKEN();
        CONDITION(lexLevel, ptrx, code, table);
        cx2 = cx;
		//JPC
        emit(8, regptr, 0, 0, code);
        if(TOKEN == 26){
            GETTOKEN();
        }
        else
            ERROR(18);
        STATEMENT(lexLevel, ptrx, code, table);
		//JMP
        emit(7, 0, 0, 0, code);
        code[cx2].m = cx;
    }

    else if(TOKEN == 31){
        GETTOKEN();
        EXPRESSION(lexLevel, ptrx, code, table);
		//SIO 1
		regptr--;
		
		if(regptr < 0 )
				regptr = 0;
			
        emit(9, regptr, 0, 1, code);
		
    }

    else if(TOKEN == 32){
        GETTOKEN();
		//SIO 2
        emit(9, regptr, 0, 2, code);
		regptr++;
        i = POS(id, ptrx, table, lexLevel);
        if(i == 0)
            ERROR(11);
        else if(table[i].kind != 2){
			//SIO 3
            emit(9, regptr, 0, 3, code);
            //ERROR(12);
            i = 0;
        }
        if(i != 0){
			//STO
			regptr--;
			if(regptr < 0 )
				regptr = 0;
            emit(4, regptr, lexLevel - table[i].level, table[i].addr, code);
		}
        GETTOKEN();
    }
}

void CONDITION(int lexLevel, int *ptrx, instructions *code, symbol *table){

    int relationSwitch;
    if(TOKEN == 8){
        GETTOKEN();
        EXPRESSION(lexLevel, ptrx, code, table);
		//ODD
        emit(15, 0, 0, 1, code); 
    }

    else{
        EXPRESSION(lexLevel, ptrx, code, table);

        if((TOKEN != 9) && (TOKEN != 10) && (TOKEN != 11) && (TOKEN != 12)
           && (TOKEN != 13) && (TOKEN != 14))
            ERROR(20);
        else{
            relationSwitch = TOKEN;
            GETTOKEN();
            EXPRESSION(lexLevel, ptrx, code, table);
            switch(relationSwitch){
            case 9:
				//EQL
                emit(17, 0, 0, 1, code);
                break;
            case 10:
				//NEQ
                emit(18, 0, 0, 1, code);
                break;
            case 11:
				//LSS
                emit(19, 0, 0, 1, code);
                break;
            case 12:
				//LEQ
                emit(20, 0, 0, 1, code);
                break;
            case 13:
				//GTR
                emit(21, 0, 0, 1, code);
                break;
            case 14:
				//GEQ
                emit(22, 0, 0, 1, code);
                break;
            }
        }
    }
}

void EXPRESSION(int lexLevel, int *ptrx, instructions *code, symbol* table) {

    int addop;
    if (TOKEN == 4 || TOKEN == 5) {
        addop = TOKEN;
        GETTOKEN();
        TERM(lexLevel, ptrx, code, table);
        if(addop == 5) {
			//NEG
            emit(10, 0, 0, 1, code);
        }
    }
    else {
        TERM (lexLevel, ptrx, code, table);
    }
    while (TOKEN == 4 || TOKEN == 5) {
        addop = TOKEN;
        GETTOKEN();
        TERM(lexLevel, ptrx, code, table);
        if (addop == 4) {
			//ADD
            emit(11, 0, 0, 1, code);
        }
        else {
			//SUB
            emit(12, 0, 0, 1, code);
        }
    }
}

void TERM(int lexLevel, int *ptrx, instructions *code, symbol* table) {

    int multop;
    FACTOR(lexLevel, ptrx, table, code);
    while(TOKEN == 6 || TOKEN == 7) {
        multop = TOKEN;
        GETTOKEN();
        FACTOR(lexLevel, ptrx, table, code);
        if(multop == 6) {
			//MUL
            emit(13, 0, 0, 1, code);
        }
        else {
			//DIV
            emit(14, 0, 0, 1, code);
        }
    }
}

void FACTOR(int lexLevel, int *ptrx, symbol* table, instructions *code) {

    int i, level, adr, val;

    while ((TOKEN == 2) || (TOKEN == 3) || (TOKEN == 15)){
        if (TOKEN == 2) {
            i = POS(id,ptrx, table, lexLevel);

          //  printf("i is %d\n", i);
            if (i == 0) {
                ERROR(11);
            }
            else {
                kind = table[i].kind;
                level = table[i].level;
                adr = table[i].addr;
                val = table[i].val;

                if (kind == 1){
					//LIT
                    emit( 1, regptr, 0, val, code);
					regptr--;
					if(regptr < 0)
						regptr = 0;
                }

                else if (kind == 2) {
					//LOD
					
                    emit(3, regptr, lexLevel - level, adr, code);
					regptr++;
                }

                else {
                    ERROR(21);
                }
            }
            GETTOKEN();
        }
        else if(TOKEN == 3){
            if (num > 2047) {
                ERROR(25);
                num = 0;
            }
			//LIT
            emit(1, regptr, 0, num, code);
			
			regptr--;
			
			if(regptr < 0)
				regptr = 0;
			GETTOKEN();
        }

        else if(TOKEN == 15){
            GETTOKEN();
            EXPRESSION(lexLevel, ptrx, code, table);

            if (TOKEN == 16) {
                GETTOKEN();
            }

            else{
                ERROR(22);
            }
        }
    }
}

void emit(int op, int r, int l, int m, instructions *code) {
    if (cx > MAX_CODE_LENGTH)
        printf("Program too long!\n");
    else {
		testCount++;
		//printf("For line #%d\nOp = %d\tL = %d\tM = %d\tR = %d\n", testCount, op, l, m, r);
        code[cx].op = op; //opcode
        code[cx].l = l;	// lexicographical level
        code[cx].m = m;	// modifier
		code[cx].r = r;	//register num
		//printf("M stored is %d\n", code[cx].m);
        cx++;
    }
}

void ENTER(int k, int *ptrx, int *pdx, int lexLevel, symbol* table) {

    char *id1;
    int j, len;
    (*ptrx)++;

    //id1 = tokenTable[lexlistx].name;
    id1 = id;
    //len = strlen(tokenTable[lexlistx].name);
    len = strlen(id);

    for (j=0; j <= len; j++) {
        table[*ptrx].name[j] =* id1;
        id1++;
    }

    table[*ptrx].kind = k;

     if (k == 1) {
        table[*ptrx].val = num;
    }

    else if (k == 2) {
        table[*ptrx].level = lexLevel;
        table[*ptrx].addr =* pdx;
        (*pdx)++;
    }

    else {
        table[*ptrx].level = lexLevel;
    }

}

void ERROR(int errorNum){

    printf("ERROR %d: ", errorNum);
    switch(errorNum){
        case 1:
            printf("Use = instead of :=\n");
            break;
        case 2:
            printf("= must be followed by a number\n");
            break;
        case 3:
            printf("Identifier must be followed by =\n");
            break;
        case 4:
            printf("Const, int, procedure must be followed by identifier\n");
            break;
        case 5:
            printf("Semicolon or comma is missing\n");
            break;
        case 6:
            printf("Incorrect symbol after procedure declaration\n");
            break;
        case 7:
            printf("Statement expected\n");
            break;
        case 8:
            printf("Incorrect symbol after statement part in block\n");
            break;
        case 9:
            printf("Period expected\n");
            break;
        case 10:
            printf("Semicolon between statements missing\n");
            break;
        case 11:
            printf("Undeclared identifier\n");
            break;
        case 12:
            printf("Assignment to constant or procedure is not allowed!\n");
            break;
        case 13:
            printf("Assignment operator expected\n");
            break;
        case 14:
            printf("Call must be followed by an identifier\n");
            break;
        case 15:
            printf("Call of a constant or variable is meaningless\n");
            break;
        case 16:
            printf("Then expected\n");
            break;
        case 17:
            printf("Semicolon or } expected\n");
            break;
        case 18:
            printf("Do expected\n");
            break;
        case 19:
            printf("Incorrect symbol following statement\n");
            break;
        case 20:
            printf("Relational operator expected\n");
            break;
        case 21:
            printf("EXPRESSION must not contain a procedure identifier\n");
            break;
        case 22:
            printf("Right parenthesis missing\n");
            break;
        case 23:
            printf("The preceding FACTOR cannot begin with this symbol\n");
            break;
        case 24:
            printf("An EXPRESSION cannot begin with this symbol\n");
            break;
        case 25:
            printf("This number is too large\n");
            break;
        case 26:
            printf("Level is larger than the maximum allowed lexicographical levels!\n");
            break;
        default:
            break;
    }
    exit(1);
}

int POS(char *id, int *ptrx, symbol* table, int lexLevel) {

    int s;
    s =* ptrx;

    int currentS;
    int diffCount = 0;
    //printf("%s,%d, %d, ", id, *ptrx, lexLevel);

    while(s != 0){
        if (strcmp(table[s].name, id) == 0) {
            if(table[s].level <= lexLevel) {
                if (diffCount != 0) {
                    previousdiff = diff;
                }

                diff = lexLevel - table[s].level;

                if(diffCount == 0){
                    currentS = s;
                }

                if (diff < previousdiff) {
                    currentS = s;
                }
                diffCount++;
            }
        }
        s--;
    }
    //printf("%d\n", currentS);
    return currentS;
}