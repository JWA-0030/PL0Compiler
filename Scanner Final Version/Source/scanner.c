//Project completed by: Chandler Douglas (ch180364) & Jesse Alsing (je973326)
//Date started: 9 October 2017
//Filename: scanner.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE_LENGTH 32000
#define MAX_NUMBER_LENGTH 5
#define MAX_IDENTIFIER_LENGTH 12

//Variable declaration for the internal representation of tokens
int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,
slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33;

//Reserved symbols for the language
char symbols[] = {'+', '-', '*', '/', ',', '(', ')', '=', '.', '<', '>', ';', ':'};

//Reserved words
char reserved[15][32] = { "const", "var", "procedure", "call", "begin", "end", "if", "then",
"else", "while", "do", "read", "write", "odd"};

//Internal representation mapping, from integer to string.
char IRMapping[34][64] = {"ZERO", "nulsym", "identsym", "numbersym", "plussym",
"minussym", "multsym", "slashsym", "oddsym", "eqlsym", "neqsym", "lessym", "leqsym",
"gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym", "periodsym",
"becomesym", "beginsym", "endsym", "ifsym", "thensym", "whilesym", "dosym", "callsym",
"constsym", "varsym", "procsym", "writesym", "readsym", "elsesym" };

FILE *ifp, *ofp;
int inputCharSize = 0, bp = 0, ip;
char buffer[11];

//Arrays for handling the lexemes
char inputChars[MAX_CODE_LENGTH], lexemeList[MAX_CODE_LENGTH], lexemeTable[MAX_CODE_LENGTH], symbolicLexemeList[MAX_CODE_LENGTH];

//Initializes the buffer array to null
void clearBuffer()
{
	int i;
	bp = 0;
	for(i = 0; i < 11; i++)
		buffer[i] = '\0';
}

//Initializes all of the lexeme arrays to null
void clearLexemeOutput()
{
	int i;
	for(i = 0; i < MAX_CODE_LENGTH; i++){
		lexemeList[i] = '\0';
		lexemeTable[i] = '\0';
		symbolicLexemeList[i] = '\0';
	}
}

//Returns the index in reserved of the string pointed to by the identifier
int reservedIndex(char* identifier)
{
	int i;

	for(i = 0; i < 15; i++){
		if(strcmp(reserved[i], identifier) == 0){
			return i;
		}
	}
	return -1;
}

//Maps an index in reserved to the proper internal representation.
int mapReserved(int spotInReserved)
{
	if (spotInReserved == 0)
		return constsym;
	if (spotInReserved == 1)
		return varsym;
    if (spotInReserved == 2)
        return procsym;
	if (spotInReserved == 3)
		return callsym;
	if (spotInReserved == 4)
		return beginsym;
	if (spotInReserved == 5)
		return endsym;
	if (spotInReserved == 6)
		return ifsym;
	if (spotInReserved == 7)
		return thensym;
    if (spotInReserved == 8)
        return elsesym;
	if (spotInReserved == 9)
		return whilesym;
	if (spotInReserved == 10)
		return dosym;
	if (spotInReserved == 11)
		return readsym;
	if (spotInReserved == 12)
		return writesym;
	if (spotInReserved == 13)
		return oddsym;

	//If we made it here we can assume that the symbol doesn't exist
    return -1;
}

//Map a symbol from the character into a representation used internally
int mapSymbol(char *symbol)
{

	if (strcmp(symbol, "+") == 0)
		return plussym;
	if (strcmp(symbol, "-") == 0)
		return minussym;
	if (strcmp(symbol, "*") == 0)
		return multsym;
	if (strcmp(symbol, "/") == 0)
		return slashsym;
	if (strcmp(symbol, "(") == 0)
		return lparentsym;
	if (strcmp(symbol, ")") == 0)
		return rparentsym;
	if (strcmp(symbol, "=") == 0)
		return eqlsym;
	if (strcmp(symbol, ",") == 0)
		return commasym;
	if (strcmp(symbol, ".") == 0)
		return periodsym;
	if (strcmp(symbol, "<") == 0)
		return lessym;
	if (strcmp(symbol, ">") == 0)
		return gtrsym;
	if (strcmp(symbol, ";") == 0)
		return semicolonsym;
	if (strcmp(symbol, "<>") == 0)
		return neqsym;
	if (strcmp(symbol, "<=") == 0)
		return leqsym;
	if (strcmp(symbol, ">=") == 0)
		return geqsym;
	if (strcmp(symbol, ":=") == 0)
		return becomessym;

	//If we made it here we can assume the symbol is not valid
    return -1;
}

/*-------------- Functions to validate the current character --------------*/

//Returns 1 if myChar is an uppercase letter, 0 otherwise.
int isUpper(char myChar)
{
	return myChar >= 65 && myChar <= 90;
}

//Returns 1 if myChar is an lowercase letter, 0 otherwise.
int isLower(char myChar)
{
	return myChar >= 97 && myChar <= 122;
}

//Returns 1 if myChar is an alpha, 0 otherwise
int isAlpha(char myChar)
{
	return isLower(myChar) || isUpper(myChar);
}

//Returns 1 if the myChar is a digit between 0-9, 0 otherwise
int isDigit(char myChar)
{
	return myChar >= 48 && myChar <= 57;
}

//Returns 1 if a character is a valid alphanumeric character, 0 otherwise
int isAlphanumeric(char myChar)
{
	return isAlpha(myChar) || isDigit(myChar);
}

//Checks to see if the character is valid whitespace or if it's invisible; returns 1 if so and 0 otherwise.
int isInvisible(char myChar)
{
	return myChar == 9 || myChar == 10 || myChar == 13 || myChar == 32;
}

//Simple check to make sure the character is a valid symbol; returns 1 if true, 0 otherwise.
int isSymbol(char myChar)
{
	int i;
	for(i = 0; i < 14; i++)
		if(symbols[i] == myChar)
			return 1;
	return 0;
}

//Returns a 1 if the character is a valid character used for scanning, 0 otherwise.
int isValid(char myChar)
{
	return isAlphanumeric(myChar) || isSymbol(myChar) || isInvisible(myChar) || myChar == '\0';
}

//Validates that the character exists & returns it as long as it does.
char getChar(int invis)
{
	char nextChar;

	if(ip > 2000)
	{
		printf("The input file ends unexpectedly!\n");
		exit(0);
	}

	nextChar = inputChars[ip];
	ip++;

	if(!invis && !isValid(nextChar))
	{
		printf("Invalid character encountered\n");
		exit(0);
	}
	return nextChar;
}

//Ungets the character in case of an error.
void ungetChar()
{
	ip--;
}

//Insert a number into the lexeme list
void insertIntToLexemeList(int num)
{
	char temp[64];

	sprintf(temp, "%d ", num);
	strcat(lexemeList, temp);
	sprintf(temp, "%s ", IRMapping[num]);
	strcat(symbolicLexemeList, temp);
}

//Insert a string into the lexeme list
void insertStrToLexemeList(char * identifier)
{
	char temp[64];

	sprintf(temp, "%s ", identifier);
	strcat(lexemeList, temp);
	strcat(symbolicLexemeList, temp);
}

//Insert the lexeme [lexeme] of type [tokenType] nicely into the lexeme table.
void insertToLexemeTable(char * lexeme, int tokenType)
{
	char temp[64];
	char spaces[64] = {'\0'};

	while(strlen(lexeme) + strlen(spaces) < 13)
		strcat(spaces, " ");
	sprintf(temp, "%s%s%d\n", lexeme, spaces, tokenType);
	strcat(lexemeTable, temp);
}

//Adds one character at a time to our buffer array
void addToBuffer(char myChar)
{
	if(bp <= 11)
	{
		buffer[bp] = myChar;
		bp++;
	}
}


//Take [identifier] and see if it's a reserved word or actually just an identifier...
void processIdentifier(char * identifier)
{
	int index = reservedIndex(identifier);
	if (index > -1)
	{
		//It's reserved!
		int mapping = mapReserved(index);
		insertIntToLexemeList(mapping);
		insertToLexemeTable(identifier, mapping);
	}
	//Print the identifier to the file
	else
	{
		//Not reserved!
		insertIntToLexemeList(2);
		insertStrToLexemeList(identifier);
		insertToLexemeTable(identifier, 2);
	}
}


//Process a number literal represented by the string pointed to by [num]
void processNumber(char * num)
{
	insertToLexemeTable(num, 3);
	insertIntToLexemeList(3);
	insertStrToLexemeList(num);
}

//Process a symbol represented by the string pointed to by [sym]
void processSymbol(char * sym)
{
	insertToLexemeTable(sym, mapSymbol(sym));
	insertIntToLexemeList(mapSymbol(sym));
}


void processText()
{
	//Clear out the output arrays...
	clearLexemeOutput();

	//Run through the input characters...
	char nextChar = ' ';
	while(nextChar != '\0')
	{
		clearBuffer();

		while(isInvisible(nextChar = getChar(0)))
		{
			//Trash the invisible characters
		}

		//It's not invisible if we are here!
		if (isAlpha(nextChar))
		{
			addToBuffer(nextChar);
			while(isAlphanumeric(nextChar = getChar(0)))
			{
				addToBuffer(nextChar);
				if (strlen(buffer) > MAX_IDENTIFIER_LENGTH)
				{
					//Invalid identifier length
					printf("Identifier too long!");
					exit(0);
				}
			}
			ungetChar();

			//Process identifier in buffer
			processIdentifier(buffer);
		}
		else if (isDigit(nextChar))
		{
			addToBuffer(nextChar);
			while(isDigit(nextChar = getChar(0)))
			{
				addToBuffer(nextChar);
				if (strlen(buffer) > MAX_NUMBER_LENGTH)
				{
					//Invalid number length
					printf("Number too long!");
					exit(0);
				}
			}
			//Was this number followed by a letter?
			if (isAlpha(nextChar))
			{
				printf("Identifier does not start with letter!");
				exit(0);
			}

			//It was not followed by a letter.. so we are okay!
			ungetChar();

			//Process number in buffer.
			processNumber(buffer);
		}
		else if (isSymbol(nextChar))
		{
			addToBuffer(nextChar);
			if (nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '(' || nextChar == ')' || nextChar == '=' || nextChar == ',' || nextChar == '.' || nextChar == ';')
			{
				//Process just that symbol itself!
				processSymbol(buffer);
			}
			else if (nextChar == '/')
			{
				nextChar = getChar(0);
				if (nextChar == '*')
				{
					state5:
					while((nextChar = getChar(1)) != '*')
					{
						//Dump comment...
					}
					while((nextChar = getChar(1)) == '*')
					{
						//Dump comment...
					}
					if (nextChar == '/')
					{
						//Done with comment!
					}
					else
					{
						//Still in comment from * to another char...
						goto state5;
					}
				}
				else
				{
					//Process as a divide symbol...
					ungetChar();
					processSymbol(buffer);
				}
			}
			else if (nextChar == '<')
			{
				nextChar = getChar(0);
				if (nextChar == '=')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else if (nextChar == '>')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else
				{
					//Process less only
					ungetChar();
					processSymbol(buffer);
				}
			}
			else if (nextChar == '>')
			{
				nextChar = getChar(0);

				if (nextChar == '=')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else
				{
					//Process greater than only
					ungetChar();
					processSymbol(buffer);
				}
			}
			else if (nextChar == ':')
			{
				//We must have an equal following it!
				nextChar = getChar(0);
				if (nextChar == '=')
				{
					addToBuffer(nextChar);
					processSymbol(buffer);
				}
				else
				{
					printf("Invalid symbol!");
					exit(0);
				}
			}

		}
		else if (nextChar == '\0')
		{
			break;
		}
		else
		{
			//Invalid state
			printf("Invalid state!");
			exit(0);
		}
	}

	//Print results to output file
	fprintf(ofp, "Symbolic lexeme list:\n");
	fprintf(ofp, "%s\n\n", symbolicLexemeList);

	fprintf(ofp, "Tokenized lexeme list:\n");
	fprintf(ofp, "%s\n", lexemeList);
}

void echoInput()
{
	fprintf(ofp, "Source Program:\n%s\n\n", inputChars);
}

int main(){
	//Open the input/output files
	ifp = fopen("scannerin.txt", "r");
	ofp = fopen("scannerout.txt", "w");

	//Ends the program if the files didn't open correctly
	if(ifp == NULL || ofp == NULL){
		printf("Please ensure that the files are located in the proper directory [same location as the source code] and the file is named: scannerin.txt\n");
		return 1;
	}

	//Seeks the end of the file
	fseek(ifp, 0, SEEK_END);
	int inputSize = ftell(ifp);
	inputCharSize = inputSize;

	//"Rewinds" the file
	fseek(ifp, 0, SEEK_SET);

	int i;
	for(i = 0; i < inputSize; i++){
		fscanf(ifp, "%c", &inputChars[i]);
	}

	//Echo the input file into the text file.
	echoInput();

	//Processes the text into PM/0 language
	processText();

	//Cleanup
	fclose(ifp);
	fclose(ofp);

	return 0;
}
