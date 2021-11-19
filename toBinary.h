#define ERROR -1
#define REG_LENGHT 3

/*strings that stores the string without a '$' ######there is possibilty that we have a problem this way##########* /
static char strREG1[2];
static char strREG2[2];
static char strREG3[2];
*/

/*declarations*/

struct fileCodingStruct;



void binPrint(unsigned long int n);
int isLabel(char * str);
int toBinary(char * commandSTR, char * operands , struct fileCodingStruct *codingData);
int findfunct(char * str);
int findOpcode(char * str);
long int Rcase3param(char * str ,char * commandSTR, struct fileCodingStruct *codingData);
long int Rcase2param(char * str ,char * commandSTR, struct fileCodingStruct *codingData);
long int Icase(char * str ,char * commandSTR, struct fileCodingStruct *codingData);
long int IcaseLabel(char * str ,char * commandSTR, struct fileCodingStruct *codingData);
long int Jcase(char * str ,char * commandSTR, struct fileCodingStruct *codingData);
int getDistAddres(int destAdd, struct fileCodingStruct *codingData);

void removeDollar(char *str, char *strREG);