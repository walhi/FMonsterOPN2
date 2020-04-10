#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vgm.h"

const char vgmMarker[4] = {0x56, 0x67, 0x6d, 0x20};

int main(int argc, char **argv)
{
	char *inputFile = NULL;
	for (int i = 0; i < argc; i++){
		if (strstr(argv[i], ".vgm") != NULL){
			inputFile = argv[i];
			break;
		}
	}

	if (inputFile == NULL){
		char test[] = "test.vgm";
		inputFile = test;
	}


	enum VGMErrorCode result = VgmOpen(inputFile);
	if (result == VGM_OK){
		fprintf(stderr, "Open complete\n");
	} else {
		fprintf(stderr, "Error: %d\n", result);

	}

	return 0;
}
