#define MIN_DB	-128
#define MAX_DB	127
#define MIN_DH	-32768
#define MAX_DH	32767
#define MIN_DW	-2147483648
#define MAX_DW	2147483647



enum print_validation_error {DONT_PRINT_ERROR, PRINT_ERROR};


struct fileCodingStruct;

int validateLabel(char *label, struct fileCodingStruct *codingData,int print_error);
int howManyComma(char * str);
int isWhiteString(char * str);
int isCorrectImmed(char * immed, struct fileCodingStruct *codingData,int print_error);
int isCorrectReg(char * reg, struct fileCodingStruct *codingData,int print_error);
int validateOperands(char *operands, int validCase, struct fileCodingStruct *codingData);
