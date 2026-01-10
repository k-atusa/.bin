#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"

#define MAX_TOKEN_LENGTH 256

// TokenVec.init
void TokenVec_init(TokenVec* arr) {
    arr->tokens = malloc(sizeof(Token) * 16);
    arr->count = 0;
    arr->capacity = 16;
}

// TokenVec.add
void TokenVec_add(TokenVec* arr, int line, TOKEN_TYPE type, char* value) {
    if (arr->count == arr->capacity) {
        arr->capacity *= 2;
        arr->tokens = realloc(arr->tokens, sizeof(Token) * arr->capacity);
        if (arr->tokens == NULL) {
            perror("Realloc failed");
            exit(EXIT_FAILURE);
        }
    }
    arr->tokens[arr->count].line = line;
    arr->tokens[arr->count].type = type;
    strcpy(arr->tokens[arr->count].value, value);
    arr->count++;
}

// TokenVec.free
void TokenVec_free(TokenVec* arr) {
    free(arr->tokens);
    arr->count = 0;
    arr->capacity = 0;
}

// TokenVec to text file
void TokenVec_save(TokenVec* arr, char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("cannot open file");
        return;
    }

    fprintf(fp, "Filename: %s\n", arr->filename);
    fprintf(fp, "Token Count: %d\n", arr->count);
    fprintf(fp, "==============================\n");

    for (int i = 0; i < arr->count; i++) {
        fprintf(fp, "Token %d:\n", i);
        fprintf(fp, "    Line: %d\n", arr->tokens[i].line);
        fprintf(fp, "    Type: %d\n", arr->tokens[i].type);
        fprintf(fp, "    Value: %s\n", arr->tokens[i].value);
    }

    fclose(fp);
}

// check numeric literal
TOKEN_TYPE isNumLiteral(char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    } else if (strcmp(str, "void") == 0) {
        return LITERAL_VOID;
    } else if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) {
        return LITERAL_BOOL;
    } else {
		int isHex = 0; // 0: decimal, 1: 0, 2: 0x
        int sawDot = 0;
        for (int i = 0; str[i] != '\0'; i++) {
            if (i == 0 && str[i] == '0') {
                isHex = 1;
            } else if (i == 1 && isHex == 1 && (str[i] == 'x' || str[i] == 'X')) {
                isHex = 2;
            } else if (str[i] == '.' && !sawDot && !isHex) {
                sawDot = 1;
            } else if (isdigit(str[i]) || (isHex == 2 && isxdigit(str[i]))) {
                continue;
            } else {
                return 0; // not a numeric literal
            }
		}
        if (isHex == 2) {
            return LITERAL_INT16; // hexadecimal literal
        } else if (sawDot) {
            return LITERAL_FLOAT; // floating-point literal
        } else {
            return LITERAL_INT10; // decimal literal
		}
    }
}

// check id
TOKEN_TYPE isNameID(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (i == 0 && '0' <= str[0] && str[0] <= '9') {
            return 0;
        }
        if (str[i] < 128 && !isalnum(str[i]) && str[i] != '_') {
            return 0;
        }
    }
    return ID_NAME;
}

// check keyword
TOKEN_TYPE isKeyword(char* str) {
    if (str == NULL) return 0;
    if (strcmp(str, "int") == 0) return KEY_INT;
    if (strcmp(str, "float") == 0) return KEY_FLOAT;
    if (strcmp(str, "bool") == 0) return KEY_BOOL;
    if (strcmp(str, "unsigned") == 0) return KEY_UNSIGNED;
    if (strcmp(str, "8bit") == 0) return KEY_8BIT;
    if (strcmp(str, "16bit") == 0) return KEY_16BIT;
    if (strcmp(str, "32bit") == 0) return KEY_32BIT;
    if (strcmp(str, "64bit") == 0) return KEY_64BIT;
    if (strcmp(str, "malloc") == 0) return KEY_MALLOC;
    if (strcmp(str, "free") == 0) return KEY_FREE;
    if (strcmp(str, "sizeof") == 0) return KEY_SIZEOF;
    if (strcmp(str, "if") == 0) return KEY_IF;
    if (strcmp(str, "else") == 0) return KEY_ELSE;
    if (strcmp(str, "while") == 0) return KEY_WHILE;
    if (strcmp(str, "return") == 0) return KEY_RETURN;
    return 0; // not a keyword
}

// check if 2-len operation
TOKEN_TYPE isDoubleOper(char* str) {
    if (str == NULL) return 0;
    if (strcmp(str, "<=") == 0) return OP_LE;
    if (strcmp(str, ">=") == 0) return OP_GE;
    if (strcmp(str, "==") == 0) return OP_EQ;
    if (strcmp(str, "!=") == 0) return OP_NE;
    if (strcmp(str, "&&") == 0) return OP_AND;
    if (strcmp(str, "||") == 0) return OP_OR;
    if (strcmp(str, "<<") == 0) return OP_BIT_LSHIFT;
    if (strcmp(str, ">>") == 0) return OP_BIT_RSHIFT;
    if (strcmp(str, "//") == 0) return COMMENT_SHORT;
    if (strcmp(str, "/*") == 0) return COMMENT_LONG;
    return 0; // not a keyword
}

// check if 1-len operation
TOKEN_TYPE isSingleOper(char c) {
    if (c == '+') return OP_ADD;
    if (c == '-') return OP_SUB;
    if (c == '*') return OP_MUL_DEREF;
    if (c == '/') return OP_DIV;
    if (c == '%') return OP_MOD;
    if (c == '<') return OP_LT;
    if (c == '>') return OP_GT;
    if (c == '!') return OP_NOT;
    if (c == '&') return OP_BIT_AND_REF;
    if (c == '|') return OP_BIT_OR;
    if (c == '~') return OP_BIT_NOT;
    if (c == '^') return OP_BIT_XOR;
    if (c == '=') return PUNC_ASSIGN;
    if (c == '#') return PUNC_SHARP;
    if (c == ';') return PUNC_SEMICOLON;
    if (c == ',') return PUNC_COMMA;
    if (c == '.') return PUNC_DOT;
    if (c == '(') return PUNC_LPAREN;
    if (c == ')') return PUNC_RPAREN;
    if (c == '{') return PUNC_LBRACE;
    if (c == '}') return PUNC_RBRACE;
    if (c == '[') return PUNC_LBRACKET;
    if (c == ']') return PUNC_RBRACKET;
    return 0; // not a keyword
}

// generate tokens from file
TokenVec tokenizeFile(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("cannot open file");
        exit(EXIT_FAILURE);
    }

    TokenVec tokens;
    TokenVec_init(&tokens);
    strcpy(tokens.filename, filename);

    char buffer[MAX_TOKEN_LENGTH];
    int bufferIndex = 0;
    int c;
    int working = 1;
    int line = 1;
    int mode = 0; // 0: normal, 1: punctuation, 2: short comment, 3: long comment, 4: char, 5: char escape, 6: string, 7: str escape

    while (working) {
        c = fgetc(fp);
        if (c == EOF) {
            working = 0;
            c = '\n';
        } else if (c == '\n') {
            line++;
        }
        if (bufferIndex >= MAX_TOKEN_LENGTH) {
            printf("E0101 token too long at %s; %d\n", filename, line);
            exit(EXIT_FAILURE);
        }

        switch (mode) {
        case 0:
            ; int endToken = 0;
            if (isspace(c)) { // end of normal token
                endToken = 1;
            } else if (c == '\'') { // start of char
                endToken = 1;
                mode = 4;
            } else if (c == '"') { // start of string
                endToken = 1;
                mode = 6;
            } else if (c < 128 && !isalnum(c) && !isspace(c) && c != '_') { // start of punctuation
                endToken = 2;
            } else { // normal character
                buffer[bufferIndex++] = c;
            }
            if (endToken) { // add token
				buffer[bufferIndex] = '\0';
                if (bufferIndex != 0) {
                    bufferIndex = 0;
                    TOKEN_TYPE type = isNumLiteral(buffer);
                    if (type) {
                        TokenVec_add(&tokens, line, type, buffer);
                    } else {
                        type = isKeyword(buffer);
                        if (type) {
                            TokenVec_add(&tokens, line, type, buffer);
                        } else {
                            type = isNameID(buffer);
                            if (type) {
                                TokenVec_add(&tokens, line, type, buffer);
                            } else {
                                printf("E0102 invalid id: %s at %s; %d\n", buffer, filename, line);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
				}
            }
			if (endToken == 2) { // punctuation or operator
                if (c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|' || c == '/') { // can be start of 2-letter operator
                    buffer[0] = c;
                    bufferIndex = 1;
                    mode = 1;
                } else {
                    TOKEN_TYPE type = isSingleOper(c);
                    buffer[0] = c;
                    buffer[1] = '\0';
                    if (type) { // single operation
                        TokenVec_add(&tokens, line, type, buffer);
                    } else { // invalid operation
                        printf("E0103 invalid operation: %s at %s; %d\n", buffer, filename, line);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            break;

        case 1: // punctuation or operator
            if (isspace(c)) { // end of punctuation
                bufferIndex = 0;
                mode = 0;
                buffer[1] = '\0';
                TokenVec_add(&tokens, line, isSingleOper(buffer[0]), buffer);
            } else if (c > 128 || c == '_' || isalnum(c)) { // start of keyword
                bufferIndex = 0;
                mode = 0;
                buffer[1] = '\0';
                TokenVec_add(&tokens, line, isSingleOper(buffer[0]), buffer);
                buffer[bufferIndex++] = c;
            } else { // second punctuation
                buffer[1] = c;
                buffer[2] = '\0';
                TOKEN_TYPE type = isDoubleOper(buffer);
                if (type) { // 2-len
                    bufferIndex = 0;
                    if (type == COMMENT_SHORT) {
                        mode = 2;
                    } else if (type == COMMENT_LONG) {
                        mode = 3;
                    } else {
                        mode = 0;
                        TokenVec_add(&tokens, line, type, buffer);
                    }
                } else if (c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|' || c == '/') { // 1-len, 1/2-len
                    buffer[1] = '\0';
                    TokenVec_add(&tokens, line, isSingleOper(buffer[0]), buffer);
                    buffer[0] = c;
                    bufferIndex = 1;
                } else {
                    type = isSingleOper(c);
                    if (type) { // 1-len, 1-len
                        mode = 0;
                        bufferIndex = 0;
                        buffer[1] = '\0';
                        TokenVec_add(&tokens, line, isSingleOper(buffer[0]), buffer);
                        buffer[0] = c;
                        TokenVec_add(&tokens, line, type, buffer);
                    } else { // invalid
                        printf("E0103 invalid operation: %s at %s; %d\n", buffer, filename, line);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            break;

        case 2: // short comment
            if (c == '\n') {
                bufferIndex = 0;
                mode = 0;
            }
            break;

		case 3: // long comment
            if (c == '*') {
                buffer[0] = '*';
            } else if (c == '/' && buffer[0] == '*') {
                bufferIndex = 0;
                mode = 0;
            } else {
                buffer[0] = 0;
            }
            break;

		case 4: // char literal
			if (c == '\'') { // end of char literal
                buffer[bufferIndex] = '\0';
                TokenVec_add(&tokens, line, LITERAL_CHAR, buffer);
				bufferIndex = 0;
				mode = 0; // back to normal mode
			} else if (bufferIndex == 0 && c == '\\') { // escape sequence
                mode++;
			} else if (c == '\n') { // unterminated char literal
                printf("E0104 unterminated char literal at %s; %d\n", filename, line);
                exit(EXIT_FAILURE);
			} else { // normal char
                buffer[bufferIndex++] = c;
			}
            break;

        case 6:
            if (c == '"') { // end of string literal
                buffer[bufferIndex] = '\0';
				TokenVec_add(&tokens, line, LITERAL_STRING, buffer);
				bufferIndex = 0;
				mode = 0; // back to normal mode
            } else if (c == '\\') { // escape sequence
                mode++;
            } else { // normal char
                buffer[bufferIndex++] = c;
			}
            break;

		case 5: case 7: // add letter to buffer
            if (c == '\\' || c == '\'' || c == '"') {
                buffer[bufferIndex++] = c;
            } else if (c == 'n') {
                buffer[bufferIndex++] = '\n';
            } else if (c == 'r') {
                buffer[bufferIndex++] = '\r';
            } else if (c == 't') {
                buffer[bufferIndex++] = '\t';
            } else if (c == '0') {
                buffer[bufferIndex++] = '\0';
            } else if (c == 'b') {
                buffer[bufferIndex++] = '\b';
            } else {
				printf("E0106 invalid escape letter: \\%c at %s; %d\n", c, filename, line);
				exit(EXIT_FAILURE);
			}
			mode--;
            break;
        }
    }

    switch (mode) { // handle unterminated token
    case 2: case 3:
        printf("W0107 unterminated comment at %s; %d\n", c, filename, line);
        break;
    case 4: case 5: case 6: case 7:
        printf("E0108 unterminated literal at %s; %d\n", c, filename, line);
        exit(EXIT_FAILURE);
        break;
    }
    return tokens;
}