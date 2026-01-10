#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>

#define MAX_TOKEN_LENGTH 256

typedef enum TOKEN_TYPE {
    LITERAL_INT10 = 1,
    LITERAL_INT16,
    LITERAL_FLOAT,
    LITERAL_BOOL,
    LITERAL_CHAR,
    LITERAL_STRING,
    LITERAL_VOID,
    ID_NAME,

    KEY_INT,
    KEY_FLOAT,
    KEY_BOOL,
    KEY_UNSIGNED,
    KEY_8BIT,
    KEY_16BIT,
    KEY_32BIT,
    KEY_64BIT,

    KEY_MALLOC,
    KEY_FREE,
    KEY_SIZEOF,
    KEY_IF,
    KEY_ELSE,
    KEY_WHILE,
    KEY_RETURN,

    OP_ADD,
    OP_SUB,
    OP_MUL_DEREF,
    OP_DIV,
    OP_MOD,

    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE,
    OP_EQ,
    OP_NE,

    OP_AND,
    OP_OR,
    OP_NOT,

    OP_BIT_AND_REF,
    OP_BIT_OR,
    OP_BIT_NOT,
    OP_BIT_XOR,
    OP_BIT_LSHIFT,
    OP_BIT_RSHIFT,

    PUNC_ASSIGN,
    PUNC_SHARP,
    PUNC_SEMICOLON,
    PUNC_COMMA,
    PUNC_DOT,

    PUNC_LPAREN,
    PUNC_RPAREN,
    PUNC_LBRACE,
    PUNC_RBRACE,
    PUNC_LBRACKET,
    PUNC_RBRACKET,

    COMMENT_SHORT,
    COMMENT_LONG
} TOKEN_TYPE;

typedef struct {
    int line;
    TOKEN_TYPE type;
    char value[MAX_TOKEN_LENGTH];
} Token;

typedef struct {
    char filename[MAX_TOKEN_LENGTH];
    Token* tokens;
    int count;
    int capacity;
} TokenVec;

void TokenVec_free(TokenVec* arr);
void TokenVec_save(TokenVec* arr, char* filename);
TokenVec tokenizeFile(char* filename);

#endif // TOKENIZER_H
