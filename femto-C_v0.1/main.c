#include <stdio.h>
#include <stdlib.h>
#include "reader.h"

// gcc reader.c main.c -o test.exe

int main() {
    char filename[256] = "test.txt";

    TokenVec tokens = tokenizeFile(filename);
    char outputFilename[256];
    sprintf(outputFilename, "%s.tokens.txt", filename);
    TokenVec_save(&tokens, outputFilename);
    TokenVec_free(&tokens);

    printf("Tokens saved to %s\n", outputFilename);
    return 0;
}