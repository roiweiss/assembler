#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "toBinary.h"
#include "commandTable.h"
#include "data.h"
#include "fileCompiler.h"
#include "validation.h"



/*function that prtinting integer in binary*/
void binPrint(unsigned long int n)
{	/* for debugging purpose */
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
    printf("\n");
}

int isLabel(char * str){
	/* recieves agrument, checks whethers its label */

	if (str[0] == '$')
		return 0;
	else if(str[0] == '-')       /*thats the only cases to be a register, all the rest cases is  lables */
		return 0;
	else if(isdigit(str[0]))
		return 0;
	else
		return 1;
}

int getDistAddres(int destAdd, fileCodingStruct *codingData){
	/* recieves label adress, and calculates distance from current encoded line's adress */

	int sourceAdd = getIC(codingData); /*find my address*/
	int res = destAdd - sourceAdd; /*this is the claculation to know where we have to go*/
	return res;
}


			
int toBinary(char * command, char * operands, fileCodingStruct *codingData) {
	/* receives valid instruction command with opernds, encodes it and send it to 'pushCode' function

	in this function we are running on codeCommands array. the numbers is each part of the cases*/

	int res = 0;
	int i = 0;
	while(strcmp(command, codeCommands[i].command)){
		i++;
	}
	if(i <= 4){  /*this is "add" "sub" "and" "or" commands*/
		res = Rcase3param(operands,command, codingData);
	}
	else if (i >= 5 && i <= 7){  /*this is "move" "mvhi" "mvlo" commands*/
		res = Rcase2param(operands,command, codingData);
	}
	else if ((i >= 8 && i <= 12) || (i >= 17 && i <= 22) ){   /*"addi"	"subi"	"andi" "ori" "nori" "lb" "sb" "lw" "sw" "lh" "sh"*/
		res = Icase(operands,command, codingData);
	}
	else if (i >= 13 && i <= 16){  /*"bne" "beq" "blt" "bgt"*/
		res = IcaseLabel(operands,command, codingData);
	}
	else if (i >= 23 && i <=26 ){  /*"jmp" "la" "call" "stop"*/
		res = Jcase(operands,command, codingData);
	}
	else
	{
		printError("invalid command",codingData);
		return 1;
	}

	return res;
}

/*void removeDollar(char * str,int occation, char *strREG1, char *strREG2, char strREG3)*/
void removeDollar(char *reg_with_dollar, char *reg_no_dollar){

	reg_no_dollar[0] = reg_with_dollar[1];
	reg_no_dollar[1] = reg_with_dollar[2];
	
}

int findfunct(char * command){
	/* recieves command's string and extracts funct from codeCommands gloabl table */
	int x;
	int i;
	int res = ERROR;

	for(i=0; i<8; i++){  /*this is the only 8 cases that we have funct*/
		if((x = strcmp(command, codeCommands[i].command)) == 0){
			res = codeCommands[i].funct;
		}
	}
	
	return res;
}

int findOpcode(char * command){	
	/* recieves command's string and extracts opcode from codeCommands gloabl table */
	int x;
	int i;
	int res = ERROR;
			/*we are running all over codeCommands array to find the opcode*/
	for(i=0; i<= CODE_COMMANDS; i++){
		if((x = strcmp(command, codeCommands[i].command)) == 0){
			res = codeCommands[i].opcode;
		}
	}
	
	return res;
}


				
long int Rcase3param(char * operands ,char * command, fileCodingStruct *codingData){
	/* encodes R commands with 3 params */
	/*Extracting each register from the string*/
	char reg1_no_dollar[REG_LENGHT] = {0};
	char reg2_no_dollar[REG_LENGHT] = {0};
	char reg3_no_dollar[REG_LENGHT] = {0};

	unsigned int reg1Val;
	unsigned int reg2Val;				
	unsigned int reg3Val;

	long int code;
	long int mask; 

	char * reg1_with_dollar  = strtok(operands,",");
	char * reg2_with_dollar = strtok(NULL,",");
	char * reg3_with_dollar = strtok(NULL,",");
	

	/*removing the '$ from the strings and saving in new pointer */
	removeDollar(reg1_with_dollar, reg1_no_dollar);      
	removeDollar(reg2_with_dollar, reg2_no_dollar);
	removeDollar(reg3_with_dollar, reg3_no_dollar);

	/*convert the string to an integer */
	reg1Val = atoi(reg1_no_dollar);
	reg2Val = atoi(reg2_no_dollar);
	reg3Val = atoi(reg3_no_dollar);


	/*coding to binary*/
	code = 0;
	mask = findOpcode(command);  /*coding opcode in each case*/
	mask <<=  26;
	code |= mask;
	mask = reg1Val;
	mask <<= 21;
	code |= mask;
	mask = reg2Val;
	mask <<= 16;
	code |= mask;
	mask = reg3Val;
	mask <<= 11;
	code |= mask;
	mask = findfunct(command);  /*coding funct in each case*/
	mask <<= 6;
	code |= mask;

	pushCode(code, codingData);  /*pushing code for printing*/
	return 0;

}

long int Rcase2param(char * str ,char * command, fileCodingStruct *codingData){
	/* encodes R commands with 2 params */
	/*Extracting each register from the string*/
	char reg1_no_dollar[REG_LENGHT] = {0};
	char reg2_no_dollar[REG_LENGHT] = {0};

	unsigned int reg1Val;
	unsigned int reg2Val;

	long int code;
	long int mask; 

	char * reg1_with_dollar  = strtok(str,",");
	char * reg2_with_dollar = strtok(NULL,",");

		/*removing the dollar*/
	removeDollar(reg1_with_dollar, reg1_no_dollar);
	removeDollar(reg2_with_dollar, reg2_no_dollar);

		/*convert the string to an integer */
	reg1Val = atoi(reg1_no_dollar);
	reg2Val = atoi(reg2_no_dollar);

	/*coding to binary*/
	code = 0;
	mask = findOpcode(command);  /*coding opcode in each case*/ 
	mask <<=  26;
	code |= mask;
	mask = reg1Val;
	mask <<= 21;
	code |= mask;
	mask = 0;
	mask <<= 16;
	code |= mask;
	mask = reg2Val;
	mask <<= 11;
	code |= mask;
	mask = findfunct(command);  /*coding funct in each case*/
	mask <<= 6;
	code |= mask;

	pushCode(code, codingData); 
	return 0;

}

long int Icase(char * str ,char * command, fileCodingStruct *codingData){
	/* encodes I commands */

	long int tempHex = 0x0000FFFF;    /*this varieble helps to initilaize immed field */
	char * reg1_with_dollar  = strtok(str,",");
	char * immed = strtok(NULL,",");
	char * reg2_with_dollar = strtok(NULL,",");
	char reg1_no_dollar[REG_LENGHT] = {0};
	char reg2_no_dollar[REG_LENGHT] = {0};
	int reg1Val;
	int reg2Val;
	short immedVal;
	long int code;   
	int opcode;
	long int mask;

		/*removing the dollar*/
	removeDollar(reg1_with_dollar, reg1_no_dollar);
	removeDollar(reg2_with_dollar, reg2_no_dollar);


	/*convert the string to an integer */
	reg1Val = atoi(reg1_no_dollar);
	reg2Val = atoi(reg2_no_dollar);		

	code = 0;
	opcode = findOpcode(command);  /*coding opcode in each case*/
	mask = opcode; 
	mask <<=  26;
	code |= mask;
	mask = reg1Val;
	mask <<= 21;
	code |= mask;
	mask = reg2Val;
	mask <<= 16;
	code |= mask;
	immedVal = atoi(immed);
	mask = tempHex;
	mask = mask & immedVal;
	code |= mask; 
	

	pushCode(code, codingData); 
	return 0;

}

long int IcaseLabel(char * str ,char * command, fileCodingStruct *codingData){
	/* encodes I commands with label */

	long int tempHex = 0x0000FFFF;    /*this varieble helps to initilaize immed field */
	char * reg1_with_dollar  = strtok(str,",");
	char * reg2_with_dollar = strtok(NULL,",");
	char * immed = strtok(NULL,",");
	char reg1_no_dollar[REG_LENGHT] = {0};
	char reg2_no_dollar[REG_LENGHT] = {0};
	int reg1Val;
	int reg2Val;
	long int code;
	int opcode;
	long int mask;
	int addressVal;
	int distanceValidation;
	short distance;  /*the distance form labels*/
	removeDollar(reg1_with_dollar, reg1_no_dollar);
	removeDollar(reg2_with_dollar, reg2_no_dollar);

	/*convert the string to an integer */
	reg1Val = atoi(reg1_no_dollar);
	reg2Val = atoi(reg2_no_dollar);

	code = 0;
	opcode = findOpcode(command);  /*coding opcode in each case*/
	mask = opcode; 
	mask <<=  26;
	code |= mask;
	mask = reg1Val;
	mask <<= 21;
	code |= mask;
	mask = reg2Val;
	mask <<= 16;
	code |= mask;

	addressVal = getLabelAdress(immed, codingData);
	if (addressVal == -1)
	{
		printError("Label was not defined!", codingData);
		return 1;
	}
	if (addressVal == 0)   /*use the extern label, print to ext file*/
	{
		printError("Illegal use of exten label", codingData);
		return 1;
	}

	distanceValidation = getDistAddres(addressVal, codingData);
	if (distanceValidation < MIN_DH || distanceValidation > MAX_DH)
	{
		printError("Distance to label too big! (can't fit 16 bits)", codingData);
		return 1;
	}
	/*distance between label address and current address fits 16 bits*/
	distance = distanceValidation;   /*moving from int to short*/
	mask = tempHex;
	mask = mask & distance;
	code |= mask;

	pushCode(code, codingData); 
	return 0;
}

long int Jcase(char * str ,char * command, fileCodingStruct *codingData){
	/* encodes J commands */

	/*Extracting each register from the string*/
	long int tempHex = 0x00FFFFFF; /*this varieble helps to initilaize immed field */
	char address[REG_LENGHT];
	short reg1Val;
	long int code;
	int opcode;
	long int mask;
	int addressVal;

	if(strcmp(command,"stop") == 0){    /*this is stop case*/
		code = 0;
		opcode = findOpcode(command);
		mask = opcode; 
		mask <<=  26;
		code |= mask;

		pushCode(code, codingData); 
		return 0;
	}

	if(!isLabel(str)){ 	/*this is register case*/								
		removeDollar(str, address);

		/*convert the string to an integer */
		reg1Val = atoi(address);

		code = 0;
		opcode = findOpcode(command);
		mask = opcode; 
		mask <<=  26;
		code |= mask;
		mask = 1;        /*this is register case*/
		mask <<= 25;
		code |= mask;
		mask = tempHex;
		mask = mask & reg1Val;
		code |= mask;

		pushCode(code, codingData); 
		return 0;
	
	}
	else {  /*this is label case*/
		addressVal = getLabelAdress(str , codingData);
		if (addressVal == -1)
		{
			printError("Label was not defined!", codingData);
			return 1;
		}
		if (addressVal == 0)  /*use the extern label, print to ext file*/
			pushExtUsage(str, codingData);

		code = 0;
		opcode = findOpcode(command);
		mask = opcode; 
		mask <<=  26;
		code |= mask;
		mask = tempHex;
		mask = mask & addressVal;
		code |= mask;

		pushCode(code, codingData); 
		return 0;

		
	}

	return 0;
	
}
