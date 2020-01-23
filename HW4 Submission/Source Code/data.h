#define MAX_CODE_LENGTH 32000
#define MAX_NUMBER_LENGTH 5
#define MAX_IDENTIFIER_LENGTH 12
#define MAX_SYMBOL_TABLE_SIZE 1000
#define MAX_LEXI_LEVELS 3
#define MAX_STACK_HEIGHT 2000

int errCount;

typedef struct {
     int op;	//Opcode
     int l;		//L
	 int r;		//R
     int m;		//M
 } instructions;

typedef struct {
    int tokenID;
    int numberValue;
    char name[12];
}tokenStruct;

tokenStruct tokenTable[MAX_SYMBOL_TABLE_SIZE];

int lexListIndex;
int tokenLength;


//Functions
void compile();
void scanner();
void parse();
void vm();
