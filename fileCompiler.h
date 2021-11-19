


struct fileCodingStruct;


int fileCompiler(char *fileName);
int encodingLineTake1(char *line, struct fileCodingStruct *codingData);
int encodingLineTake2(char *line, struct fileCodingStruct *codingData);

int readFileLine(FILE *file, char *line, int *reachedEOF, struct fileCodingStruct *codingData);
int seperateArguments(char *line, char *lable, char *command, char *operands, struct fileCodingStruct *codingData);
int operandPointers(char *line, int *start, int *end);
int countOperands(char *operands);
int getStringLenght(char *operands);
void removeWhites(char *operands);
void advanceImageCounter(char *command, char *operands, struct fileCodingStruct *codingData);

int createObjectFile(char *objectFileName, struct fileCodingStruct *codingData);
int createExtFile(char *extFileName, struct fileCodingStruct *codingData);
int createAndFillEnt(struct fileCodingStruct *codingData);

void printSymbolTable(char *fileName, struct fileCodingStruct *codingData);
void printTake(char *lable, char *command, char *operands, struct fileCodingStruct *codingData);
void printCountersBefore(struct fileCodingStruct *codingData);
void printCountersAfter(struct fileCodingStruct *codingData);