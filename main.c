#include <stdio.h>
#include <string.h>
#include "fileCompiler.h"
#include "data.h"

int validateFileName(char *fileName);


int main(int argc, char const *argv[])
{
	
	int i, j, isDuplicate, errorCounter=0;
	char fileName[FILE_NAME_SIZE];
	if (argc == 1)
		printf("No files were recieved for compilation!\n");
	else
	{
		for (i=1; i<=argc-1; i++)
		{
			strcpy(fileName, argv[i]);
			isDuplicate = 0;
			for (j=1; j<i; j++)
				if (strcmp(fileName,argv[j]) == 0)
					isDuplicate = 1;

			if (!isDuplicate)
			{
				if (validateFileName(fileName) != 0)
					printf("%s is not a valid assembly file name!\n", fileName);
				else
					errorCounter += fileCompiler(fileName);
			}

		}
	}

	if (errorCounter > 0)
		printf("compilation failed with %i errors!\n", errorCounter);

	return errorCounter;
}



int validateFileName(char *fileName)
{
	int i;
	for (i=0; fileName[i]!=0; i++)
	{
		if (i!=0 && fileName[i] == '.')
			if (strcmp((fileName+i), ".as") == 0)
				return 0;
	}
	return 1;
}