#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

int scannerFlag, vmFlag, parserFlag, temp;
FILE *ifp;

/*const char *lexSymbols[] = { "", "nulsym", "identsym", "numbersym", "plussym", "minussym", "multsym",  "slashsym",
"oddsym", "eqsym", "neqsym", "lessym", "leqsym", "gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym",
"semicolonsym", "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym", "whilesym", "dosym",
"callsym", "constsym", "varsym", "procsym", "writesym", "readsym" , "elsesym"};
*/

void compile();

int main(int argc, char *argv[]){

    while(argc > 1){
        if(strcmp(argv[1], "-l") == 0)
            scannerFlag = 1;
        if(strcmp(argv[1], "-a") == 0)
            parserFlag = 1;
        if(strcmp(argv[1], "-v") == 0)
            vmFlag = 1;
        argc--;
        argv++;
    }
    compile();
}

void compile() {

    scanner();
    parse();
    
	/* VM output not running correctly because the parser isn't working correctly */
	vm();

    if (scannerFlag == 1) {
        printf("\n");
        ifp = fopen("scanOut.txt", "r");
        while((temp = fgetc(ifp)) != EOF){
            printf("%c", temp);
        }
        printf("\n");
        fclose(ifp);
    }

    if (parserFlag == 1){
        ifp = fopen("parseOut.txt", "r");
        while((temp = fgetc(ifp)) != EOF){
            printf("%c", temp);
        }
        printf("\n");
        fclose(ifp);
    }

    if (vmFlag == 1){
        ifp = fopen("vmOut.txt", "r");
        while((temp = fgetc(ifp)) != EOF){
            printf("%c", temp);
        }
        printf("\n");
        fclose(ifp);
    }
        if(errCount == 0){
        printf("No errors, program is syntactically correct\n\n");
    }
}
