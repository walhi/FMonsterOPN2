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

  result = VgmOpen(inputFile);
  if (result == VGM_OK){
    fprintf(stderr, "Open complete\n");
  } else {
    fprintf(stderr, "Error: %d\r\n", result);
  }

  return 0;
}
