#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgm.h"

int main(int argc, char **argv)
{
	char *inputFile = NULL;
	int i;
	enum VGMErrorCode result;
	for (i = 0; i < argc; i++){
		if (strstr(argv[i], ".vgm") != NULL){
			inputFile = argv[i];
			break;
		}
	}

	if (inputFile == NULL)
		return 1;

	printf("%d\r\n", -12);
	fprintf(stderr, "%d\r\n", -13);
	printf("0x%02x\r\n", (char)-14);
	result = VgmOpen(inputFile);
	if (result == 0){
		fprintf(stderr, "Open complete\n");
	} else {
		fprintf(stderr, "Error: 0x%02x |%d|\r\n", result, (int)result);
	}

	return 0;
}
