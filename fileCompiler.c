#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "data.h"
#include "fileCompiler.h"
#include "toBinary.h"
#include "commandTable.h"
#include "validation.h"
#include "debugFlags.h"




int fileCompiler(char *fileName)
{	/* recieves assembly source code file name in fileName.as format and performs compiler's encoding part.
	if no compilations errors founds the function will:
		create fileName.ob with encoded commands (hexa)
		create fileName.ext with adresses of commands that used extern defined labels
		create fileName.ent with all defined entry adresses in 
		return 0
	if found compilation errors, no files will be created, and the function will return number of errors if detected */

	FILE *sourceFile;
	char line[LINE_LENGTH+1];
	char objectFileName[FILE_NAME_SIZE];
	char extFileName[FILE_NAME_SIZE+1];
	int reachedEOF, errorCounter = 0;
	fileCodingStruct codingData;
	

	sourceFile = fopen(fileName, "r");

	if (sourceFile == NULL)
	{
	/*	failed to open source file */
		printf("Failed to open %s\n",fileName);
		return 1;
	}
	

	/* initialize counters and symbol table's linked list head, and entering file name to coding data struct, */
	resetCounterParams(&codingData);
	codingData.symbolLinkHead = NULL;
	strcpy(codingData.fileName, fileName);


	/* First time going over source code */
	reachedEOF = 0;
	while (!reachedEOF)
	{	/* goes over the source file, line by line, and sends each legal lenght line to encodingLineTake1
		counts detected compiling errors */

		/* readFileLine reads current line from file, and advances file pointer to next line */
		if (readFileLine(sourceFile, line, &reachedEOF, &codingData) == 0)
			errorCounter += encodingLineTake1(line, &codingData);
		else
		{
			printError("Line too long", &codingData);
			errorCounter ++;
		}
		codingData.sourceLine ++;
	}


	if (errorCounter != 0) /* errors were found after first time going over source code */
	{
		if (SHOW_GENERAL) printf("Found errors in Take1, aborting compilation for %s\n", fileName); /* debug print */
		freeSymbolTable(&codingData);
		fclose(sourceFile);
		return errorCounter;
	}


	if (SHOW_GENERAL) printf("\nGreat Success!! Finished Take1! \n\n"); /* debug print */

	finalizeSymbolTable(&codingData);

	/* for debugging - using SHOW_SYMBOL/IC/DC macros */
	if (SHOW_FINAL_COUNTERS) printf(BOLDRED"$$$\t ICF:%i  DCF:%i \t$$$\n"RESET, codingData.icf, codingData.dcf);
	if (SHOW_SYMBOL_TABLE) printSymbolTable(fileName, &codingData);


	/* Setting up for second read of source code: */

	fseek(sourceFile, 0, SEEK_SET);

	if (createObjectFile(objectFileName, &codingData))
	{	/* failed creating file */
		freeSymbolTable(&codingData);
		fclose(sourceFile);
		return 1;
	}

	if (createExtFile(extFileName, &codingData))
	{	/* failed creating file */
		freeSymbolTable(&codingData);
		fclose(codingData.objectFile);
		fclose(sourceFile);
		return 1;
	}

	/* print IC and DC to object file */
	fprintf(codingData.objectFile, "%i %i\n", codingData.icf-100, codingData.dcf);

	createDataImage(&codingData);
	resetCounterParams(&codingData);

	/* Second time going over source code */
	reachedEOF = 0;
	while (!reachedEOF)
	{	/* goes over the source file, line by line, and sends each legal lenght line to encodingLineTake2
		counts detected compiling errors */

		/* readFileLine reads current line from file, and advances file pointer to next line */
		if (readFileLine(sourceFile, line, &reachedEOF, &codingData) == 0)
			errorCounter += encodingLineTake2(line, &codingData);
		codingData.sourceLine ++;
	}
	

	if (SHOW_GENERAL) printf("\nGreat Success!! Finished Take2! \n\n"); /* debug print */
	/* for debugging - using SHOW_SYMBOL/IC/DC macros */
	if (SHOW_SYMBOL_TABLE) printSymbolTable(fileName, &codingData);
	
	fclose(sourceFile);
	fclose(codingData.extFile);

	/* writing data immage array's content to object file */
	dataImageToFile(&codingData);
	fclose(codingData.objectFile);

	if (errorCounter == 0)
		errorCounter = createAndFillEnt(&codingData);

	freeDataImage(&codingData);
	freeSymbolTable(&codingData);

	if (errorCounter != 0 || codingData.externUsed == 0) 
		/* found compoling errors on Take2, or no extern labels were used */
		remove(extFileName); /* deletes .ext file */

	if (errorCounter != 0) /* deletes .ob file if found erorrs on Take 2 */
		remove(objectFileName);
	

	return errorCounter;
}

int encodingLineTake1(char *line, struct fileCodingStruct *codingData)
{/* 1. recieves source code line, parses it and separate it to label, command and operands
	2. performs validation for label, command and operans
	3. pushes defined labels to symbol table (only labels defined at beginning of line, and extern declerations)
	retuns 0 on success, 1 if compiling error found */

	char lable[LABEL_SIZE] = {0};
	char command[COMMAND_SIZE] = {0};
	char operands[LINE_LENGTH] = {0};

	int returnVal;

	/* parse line into lable, command and operands buffers
	if no line/operands detected buffer will remain an empty string */
	returnVal = seperateArguments(line, lable, command, operands, codingData);

	if (returnVal == 1) /* error detected */
		return 1;
	else if (returnVal != 0) /* blank or comment line */
		return 0; 

	/* now lable, command, and operands strings are seperated*/

	/* for debugging - using SHOW_LABLE/COMMAND/OPERANDS macros */
	if (SHOW_TAKE == 1 || SHOW_TAKE == 3) printTake(lable, command, operands, codingData);
	
	if (analyzeCommand(command, codingData))
	{
		printError("illegal command", codingData);
		return 1;
	}

	/* codingDataStruct now contains relevant fields for encoding and validation */

	/* VALIDATE_OPERANDS is debugging flag to give us option to turn off validation */
	if (VALIDATE_OPERANDS && validateOperands(operands, codingData->validationCase, codingData))
		return 1;

	lable[strlen(lable)-1] = 0; /* removing ':' at end of lable */


	if (strcmp(lable,""))
	{	/* lable was defined */
		
		/* VALIDATE_LABLE is debugging flag to give us option to turn off validation */
		if (VALIDATE_LABLE && validateLabel(lable, codingData, PRINT_ERROR))
			return 1;

		if ((strcmp(".extern",command) == 0))
			printWarning("Ignoring label definition before .extern command", codingData);
		else if ((strcmp(".entry",command) == 0))
			printWarning("Ignoring label definition before .entry command", codingData);
		else
		{
			/* pushing lable to symbol table */
			if (pushLable(lable, codingData->imageType, INTERN, codingData))
				return 1;
		}
	}

	if ((strcmp(".extern",command) == 0))
	{	/* lable was declared as extern */
		if (pushLable(operands, codingData->imageType, EXTERN, codingData))
			return 1;
	}


	/* for debugging - using SHOW_IC/DC macros */
	if ((SHOW_TAKE == 1 || SHOW_TAKE == 3) && (SHOW_IC || SHOW_DC)) printCountersBefore(codingData);

	/* Advancing IC/DC, depends on detected command */
	advanceImageCounter(command, operands, codingData);

	/* for debugging - using SHOW_IC/DC macros */
	if ((SHOW_TAKE == 1 || SHOW_TAKE == 3) && (SHOW_IC || SHOW_DC)) printCountersAfter(codingData);


	return 0;
}

int encodingLineTake2(char *line, struct fileCodingStruct *codingData)
{/* 1. recieves source code line, parses it and separate it to label, command and operands
	2. label, command and operans already validated
	3. encodes instructions commands and pushes them to output .ob file
	4. pushes entry labels to symbol table
	retuns 0 on success, 1 if compiling errors found */

	char lable[LABEL_SIZE] = {0};
	char command[COMMAND_SIZE] = {0};
	char operands[LINE_LENGTH] = {0};

	int returnVal;

	/* parse line into lable, command and operands buffers
	if no line/operands detected buffer will remain an empty string */
	returnVal = seperateArguments(line, lable, command, operands, codingData);
	if (returnVal == 1) /* error detected */
		return 1;
	else if (returnVal != 0) /* blank or comment line */
		return 0; 

	/* now lable, command, and operands strings are seperated*/
	/* operands were validated at Take1 */
	if (strcmp(".asciz",command) != 0)
		removeWhites(operands);
	
	/* for debugging - using SHOW_LABLE/COMMAND/OPERANDS macros */
	if (SHOW_TAKE == 2 || SHOW_TAKE == 3) printTake(lable, command, operands, codingData);

	if (analyzeCommand(command, codingData))
	{
		printError("illegal command", codingData);
		return 1;
	}

	/* codingDataStruct now contains relevant fields for encoding and validation */

	if ((strcmp(".entry",command) == 0))
	{	/* lable was declared as entry */
		if (pushLable(operands, codingData->imageType, ENTRY, codingData))
			return 1;
	}

	/* Deal with encoding functions */

	if (codingData->imageType == CODE_IMAGE) /* dealing with instruction command */
		if (toBinary(command, operands, codingData))
			return 1; /* found error while encoding */

	if (codingData->imageType == DATA_IMAGE) /* dealing with data command */
	{
		/* for debugging - printing data images's index */
		if (SHOW_DATA_ARR_I) printf("\tdata image index: %i->", codingData->dataImage->currIndex);

		if (strcmp(".asciz",command) == 0)
			pushDataStr(operands, codingData);

		else /* command is .db/.dh/.dw */
			pushDataInt(operands, countOperands(operands), codingData);

		/* for debugging - printing data images's index */
		if (SHOW_DATA_ARR_I) printf("%i\n", codingData->dataImage->currIndex);
	}
	
	/* for debugging - using SHOW_IC/DC macros */
	if ((SHOW_TAKE == 2 || SHOW_TAKE == 3) && (SHOW_IC || SHOW_DC)) printCountersBefore(codingData);

	/* Advancing IC/DC, depends on detected command */
	advanceImageCounter(command, operands, codingData);

	/* for debugging - using SHOW_IC/DC macros */
	if ((SHOW_TAKE == 2 || SHOW_TAKE == 3) && (SHOW_IC || SHOW_DC)) printCountersAfter(codingData);

	return 0;
}

int readFileLine(FILE *file, char *line, int *reachedEOF, fileCodingStruct *codingData)
{	/* recieved file pointer points at beginning of a line.
	the function reads current line from file and writes it into given 'line' buffer
	given 'reachedEOF' flag will be turned on in EOF detected
	at the end of the function, file pointer will point to beginning of next line
	returns 1 if parsed line was too long, 0 if not */
	int c, i = 0;

	while ((c=fgetc(file)) != EOF && c!='\n')
	{
		if (i >= LINE_LENGTH)
		{
			while ((c=fgetc(file)) != EOF && c!='\n');
			/* finish reading rest of the line*/
			if (c == EOF)
				*reachedEOF = 1;
			return 1;
		}

		line[i] = c;
		i++;
	}

	if (c == EOF)
		*reachedEOF = 1;
	line[i] = 0;

	return 0;
}

int seperateArguments(char *line, char *lable, char *command, char *operands, struct fileCodingStruct *codingData)
{	/* recieves source code line to parse, and then extracts label, command and operands out of it
	each parsed argument will be copied to label/command/oprands buffers with no whitenotes at the begiining/end
	return values:
	0 - line, label and commands successfully parsed out of line with no errors
	1 - compilation error found
	2 - detected line with only white notes
	3 - detected comment line */

	int start=0, end, reachedNULL, lastCharIndex;
	lastCharIndex = strlen(line)-1;


	if (SHOW_LINE) printf("Parsing line: '%s'\n", line); /* debug print */

	/* setting start and end indexes to wrap first argument */
	reachedNULL = operandPointers(line, &start, &end);

	if (line[start] == 0) /* line contains only white notes */
		return 2;

	if (line[start] == ';') /* comment line */
		return 3;

	if (line[end-1] == ':')
	{
		/* First argument is a label */
		if (end-start > LABEL_SIZE)
		{
			printError("label too long", codingData);
			return 1;
		}
		if (end-start == 1)
		{
			printError("illegal label definition", codingData);
			return 1;
		}
		line[end] = 0;
		strcpy(lable, line+start);

		if (reachedNULL)
		{
			printError("missing command", codingData);
			return 1;
		}
		start = end+1;
		
		/* setting start and end indexes to wrap next argument (command) */
		reachedNULL = operandPointers(line, &start, &end);
		if (line[start] == 0)
		{
			printError("missing command", codingData);
			return 1;
		}
	}

	/* start and end indexes now wrapping the command start/end */
	if (end-start > COMMAND_SIZE)
	{
		printError("invalid command", codingData);
		return 1;
	}

	line[end] = 0;
	strcpy(command, line+start);
	if (reachedNULL) /* Nothings to parse after the command */
		return 0;

	/* parsing operands into 'operands', with no whitenotes at the beginning and end */
	start = end+1;
	while (isspace(line[start]))
		start++;

	end = lastCharIndex;
	while (isspace(line[end]))
	{
		line[end] = 0;
		end --;
	}
	strcpy(operands, line+start);

	return 0;
}

int operandPointers(char *line, int *start, int *end)
{/*	function gets a string with source code line as string, a starting and ending indexes in the string.
	start index will be moved to the first none white character.
	end index will be moved to first white character/NULL after start index.
	returns 1 if end points at end of line, 0 if not */

	while (isspace(line[*start]))
		(*start)++;

	*end = *start;
	while (line[*end]!=0 && !isspace(line[*end]))
		(*end)++;

	if (line[*end]==0)
		return 1;

	return 0;

}

int countOperands(char *operands)
{	/* assumes operands are valid, counting operands by counting commas */
	int i=0, commaCounter = 0;

	while (operands[i] != 0)
	{
		if (operands[i] == ',')
			commaCounter ++;

		i++;
	}

	return commaCounter+1;
}

int getStringLenght(char *operands)
{	/*recieves string in the format "STRING"
	lenght take into considaration place for NULL, and ignores quatation marks */

	return strlen(operands)-1;
}

void removeWhites(char *operands)
{/*	removes white notes from string */
	int i,j;

	for (i=0,j=0; operands[j]!=0;j++)
	{
		if (!isspace(operands[j]))
		{
			operands[i] = operands[j];
			i++;
		}
	}
	operands[i] = 0;
}

void advanceImageCounter(char *command, char *operands, fileCodingStruct *codingData)
{	/* recieves parsed command and operands from souce code's line
	uses imageType and commandImageBytes from codingDataStcut, after updated by analyzeCommand
	advances ic or dc, depends on detected command in current line*/
	if (codingData->imageType == CODE_IMAGE)
		codingData->ic += 4;

	if (codingData->imageType == DATA_IMAGE)
	{
		if (strcmp(".asciz",command) == 0) /* .asciz command */
			codingData->dc += codingData->commandImageBytes * getStringLenght(operands);
		else /* .db/.dh./.dw command */
			codingData->dc += codingData->commandImageBytes * countOperands(operands);
	}
}

int createObjectFile(char *objectFileName, struct fileCodingStruct *codingData)
{
	strcpy(objectFileName, codingData->fileName);
	/* change .as to .ob */
	objectFileName[strlen(objectFileName)-2] = 'o';
	objectFileName[strlen(objectFileName)-1] = 'b';

	codingData->objectFile = fopen(objectFileName, "w");
	if (codingData->objectFile == NULL)
	{
		printf("Failed to open %s for writing\n",objectFileName); /* with to printError ######################### */
		return 1;
	}

	return 0;
}

int createExtFile(char *extFileName, struct fileCodingStruct *codingData)
{
	int lastCharIndex;

	codingData->externUsed = 0;

	strcpy(extFileName, codingData->fileName);
	/* change .as to .ext */
	lastCharIndex = strlen(extFileName)-1;

	extFileName[lastCharIndex-1]	= 'e';
	extFileName[lastCharIndex]		= 'x';
	extFileName[lastCharIndex+1]	= 't';
	extFileName[lastCharIndex+2]	=  0;

	codingData->extFile = fopen(extFileName, "w");
	if (codingData->extFile == NULL)
	{
		printf("Failed to open %s for writing\n",extFileName);
		return 1;
	}

	return 0;

}

int createAndFillEnt(struct fileCodingStruct *codingData)
{	/* if entry labels were defined, creates .ent file and writes them there */
	char entFileName[FILE_NAME_SIZE];
	
	FILE *entFile;
	symbolLink *currLink;

	int lastCharIndex;
	int printedEnt = 0;

	strcpy(entFileName, codingData->fileName);
	/* change .as to .ent */
	lastCharIndex = strlen(entFileName)-1;
	entFileName[lastCharIndex-1]	= 'e';
	entFileName[lastCharIndex]		= 'n';
	entFileName[lastCharIndex+1]	= 't';
	entFileName[lastCharIndex+2]	=  0;


	entFile = fopen(entFileName, "w");
	if (entFile == NULL)
	{
		printf("Failed to open %s for writing\n",entFileName);
		return 1;
	}

	currLink = codingData->symbolLinkHead;
	/* writes all entry labels to .ent file */
	while (currLink)
	{
		if (currLink->visibility == ENTRY)
		{
			fprintf(entFile, "%s %04i\n", currLink->name, currLink->adress);
			printedEnt ++;
		}

		currLink = currLink->next;
	}

	fclose(entFile);
	if (printedEnt == 0) /* deletws file if no entry entry lables were defined */
		remove(entFileName);
	return 0;

}

void printSymbolTable(char *fileName, fileCodingStruct *codingData)
{	/* debug print - prints symbol table content */
	symbolLink *currLink;

	printf(BOLDYELLOW"$$$ %s: SybmbolTable after Take1: $$$\n"RESET, fileName);
	currLink = codingData->symbolLinkHead;
	while (currLink)
	{
		printf("name: '%s'\t", currLink->name);
		printf("adress: '%i'\t", currLink->adress);
		printf("Attributes: ");
		if (currLink->placing == CODE_IMAGE) printf("code ");
		if (currLink->placing == DATA_IMAGE) printf("data ");
		if (currLink->visibility == ENTRY) printf("entry");
		if (currLink->visibility == EXTERN) printf("external");

		printf("\n");

		currLink = currLink->next;
	}
}

void printTake(char *lable, char *command, char *operands, fileCodingStruct *codingData)
{	/* debug print - print parsed label, command and operands */
	printf(BOLDYELLOW"line %i:"RESET, codingData->sourceLine);

	if (SHOW_LABLE)
		printf("\tlable:"BOLDWHITE"'%s'"RESET, lable);
	if (SHOW_COMMAND)
		printf("\tcommand:"BOLDWHITE"'%s'"RESET, command);
	if(SHOW_OPERANDS)
		printf("\toperands:"BOLDWHITE"'%s'"RESET, operands);

	printf("\n");
}

void printCountersBefore(fileCodingStruct *codingData)
{	/* debug pring - prints IC/DC before advanceImageCounter */

	if (SHOW_IC && codingData->imageType == CODE_IMAGE)
		printf(BOLDRED"\tic: %i->", codingData->ic);
	if (SHOW_DC && codingData->imageType == DATA_IMAGE)
		printf(BOLDRED"\tdc:%i->", codingData->dc);
}

void printCountersAfter(fileCodingStruct *codingData)
{	/* debug pring - prints IC/DC after advanceImageCounter */

	if (SHOW_IC && codingData->imageType == CODE_IMAGE)
		printf("%i\n"RESET, codingData->ic);
	if (SHOW_DC && codingData->imageType == DATA_IMAGE)
		printf("%i\n"RESET, codingData->dc);
}
