#ifndef READER_H
#define READER_H

#define MAX_TOKEN_LENGTH 256

// token struct
typedef struct {
    int line;
    int type;
    char value[MAX_TOKEN_LENGTH];
} Token;

// token vector
typedef struct {
    char filename[MAX_TOKEN_LENGTH];
    Token* tokens;
    int count;
    int capacity;
} TokenVec;

// TokenVec.init
void TokenVec_init(TokenVec* arr);

// TokenVec.add
void TokenVec_add(TokenVec* arr, int line, int type, char* value);

// TokenVec.free
void TokenVec_free(TokenVec* arr);

// TokenVec to text file
void TokenVec_save(TokenVec* arr, char* filename);

// generate tokens from file
TokenVec tokenizeFile(char* filename);

#endif