//Project completed by: Chandler Douglas (ch180364) & Jesse Alsing (je973326)
//Date started: 25 September 2017
//Project Name: vm.c

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "data.h"

struct instruction{
	int op; //operation
	int r; 	//register
	int l; 	//lexi level
	int m; 	//depends on op: a number/program address/data address/register in arithmetic & logic instructions
};

//Global variables for ease of computing
int stack[MAX_STACK_HEIGHT];
//instructions code[MAX_CODE_LENGTH];

char *const OPCodes[] = {"", "lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "neg", "add", "sub", "mul", "div", "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"};
int halt = 0;
int pc, sp, bp;
int R[8] = {0};

//Function prototypes
void runvm(FILE *ifp, FILE *ofp);
void run(struct instruction IR, FILE *ofp);
void printStack(struct instruction IR, FILE *ofp);
int base(int l, int base);

void printStackFrame(int SP, int BP, FILE* ofp);

void vm(){

	//Input/Output files
	FILE *ifp, *ofp;
	ifp = fopen("parseOut.txt", "r");
	ofp = fopen("vmOut.txt", "w");

	//Error and print a friendly message to the screen if the file was not able to be opened
	if(ifp == NULL || ofp == NULL){
		printf("The file was not found. Please ensure the filenames is correct (vminput.txt) and the file is in the proper directory\n");
		return ;
	}

	//Run the vm-machine with the input file given.
	runvm(ifp, ofp);

	//Cleanup - close both files
	fclose(ifp);
	fclose(ofp);
	
}

//Outer function called in main - runs the vm machine
void runvm(FILE *ifp, FILE *ofp){
	struct instruction IR[MAX_CODE_LENGTH];
	struct instruction code[MAX_CODE_LENGTH];
	int i, count = 0;

	//Initialize the stack to only 0's
	for(i = 0; i < MAX_STACK_HEIGHT; i++)
		stack[i] = 0;

	//Intialize register values - everything is 0 except for the base pointer (default is 1)
	sp = 0;
	bp = 1;
	pc = 0;

	//Read the file into the instruction array
	while(fscanf(ifp, "%d%d%d%d", &code[count].op, &code[count].r, &code[count].l, &code[count].m) != EOF){
		count++;
	}

	//Setup the output file in the proper format
    fprintf(ofp, "Line  OP      R    L    M\n");
    for(i = 0; i < count; i++){
        fprintf(ofp, "%3d   %3s   %3d  %3d  %3d\n", i, OPCodes[code[i].op], code[i].r, code[i].l, code[i].m);
    }
    fprintf(ofp,"\n");
    fprintf(ofp,"\t\t\t\t\t\tpc  bp  sp  \tstack\n");
    fprintf(ofp,"Initial Values   \t\t0   1   0 \t-EMPTY-\n");

	//Fetch-excecute cycle
	while(!halt){
		IR[pc] = code[pc];
		pc++;
		fprintf(ofp, "%3d %3s %3d %3d %3d", pc - 1, OPCodes[code[pc - 1].op], code[pc - 1].r, code[pc - 1].l, code[pc - 1].m);

        //Inner function
		run(IR[pc - 1], ofp);

		//Print to file
        fprintf(ofp, "\t\t%2d%3d%3d", pc, bp, sp);
        fprintf(ofp, "\t");

        printStackFrame(sp, bp, ofp);
        fprintf(ofp, "\n");
        fprintf(ofp, "RF: %d, %d, %d, %d, %d, %d, %d, %d\n", R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7]);
	}
}

//Executes the correct instruction depending on the input given to it
void run(struct instruction IR, FILE *ofp){
	switch(IR.op){
		//LIT
		case 1:
			R[IR.r] = IR.m;
			break;
		//RTN
		case 2:
			sp = bp - 1;
			bp = stack[sp + 3];
			pc = stack[sp + 4];
			break;
		//LOD
		case 3:
			R[IR.r] = stack[base(IR.l, bp) + IR.m];
			break;
		//STO
		case 4:
			stack[base(IR.l, bp) + IR.m] = R[IR.r];
			break;
		//CAL
		case 5:
			stack[sp + 1] = 0;
			stack[sp + 2] = base(IR.l, bp);
			stack[sp + 3] = bp;
			stack[sp + 4] = pc;
			bp = sp + 1;
			pc = IR.m;
			break;
		//INC
		case 6:
			sp = sp + IR.m;
			break;
		//JMP
		case 7:
			pc = IR.m;
			break;
		//JPC
		case 8:
			if(R[IR.r] == 0){
				pc = IR.m;
			}
			break;

		//SIO
		case 9:
		    //Print the current register to screen
            if(IR.m == 1){
                printf("%d\n", R[IR.r]);
            }
            //Read the input from the console
            else if(IR.m == 2){
                scanf("%d", &R[IR.r]);
            }
            //End the program
            else{
                halt = 1;
            }
			break;
		//NEG
		case 10:
			R[IR.r] = -1 * R[IR.r];
			break;
		//ADD
		case 11:
			R[IR.r] = R[IR.l] + R[IR.m];
			break;
		//SUB
		case 12:
			R[IR.r] = R[IR.l] - R[IR.m];
			break;
		//MUL
		case 13:
			R[IR.r] = R[IR.l] * R[IR.m];
			break;
		//DIV
		case 14:
			R[IR.r] = R[IR.l] / R[IR.m];
			break;
		//ODD
		case 15:
			R[IR.r] %= 2;
			break;
		//MOD
		case 16:
			R[IR.r] = R[IR.l] % R[IR.m];
			break;
		//EQL
		case 17:
			if(R[IR.l] == R[IR.m]) R[IR.r] = 1;
			else R[IR.r] = 0;
			break;
		//NEQ
		case 18:
			if(R[IR.l] != R[IR.m]){R[IR.r] = 1;}
			else R[IR.r] = 0;
			break;
		//LSS
		case 19:
			if(R[IR.l] < R[IR.m]) R[IR.r] = 1;
			else R[IR.r] = 0;
			break;
		//LEQ
		case 20:
			if(R[IR.l] <= R[IR.m]) R[IR.r] = 1;
			else R[IR.r] = 0;
			break;
		//GTR
		case 21:
			if(R[IR.l] > R[IR.m]) R[IR.r] = 1;
			else R[IR.r] = 0;
			break;
		//GEQ
		case 22:
			if(R[IR.l] >= R[IR.m]) R[IR.r] = 1;
			else R[IR.r] = 0;
			break;
		default:
			break;
	}
}

void printStackFrame(int SP, int BP, FILE* ofp) {

    int i = 0;

    //If BP is 0, stack has finished
    if (BP == 0) {
        return;
    }

    //If BP is 1, we're in the main stack frame. Can return.
    else if (BP == 1) {
        for(i = 1;i <= SP; i++){
                fprintf(ofp, "%d ",stack[i]);
        }
        return;
    }

    //Recursive Case: Prints out each new stack frame, separating them with |
    else {
        printStackFrame(BP - 1, stack[BP + 2], ofp);

        //Covers the case of where a CAL condition was met
        if (SP < BP) {
                fprintf(ofp, "| ");

            for (i = 0; i < 4; i++) {
                    fprintf(ofp, "%d ", stack[BP + i]);
            }
        }

        //Majority case
        else {
            fprintf(ofp, "| ");

            for (i = BP; i <= SP; i++) {
                    fprintf(ofp, "%d ", stack[i]);
            }
        }
        return;
    }
}

//Given - returns the base L levels down
int base(int l, int base)
{
	int b1;
	b1 = base;
	while (l > 0){
		b1 = stack[b1 + 1];
		l--;
	}
	return b1;
}
