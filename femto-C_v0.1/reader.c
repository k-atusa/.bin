#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 256

// token types : literal and id
#define LITERAL_INT10 1
#define LITERAL_INT16 2
#define LITERAL_FLOAT 3
#define LITERAL_BOOL 4
#define LITERAL_CHAR 5
#define LITERAL_STRING 6
#define ID_NAME 7

// token types : keywords
#define KEY_U 11 // u8 u16 u32 u64 uint
#define KEY_I 12 // i8 i16 i32 i64 int
#define KEY_F 13 // f32 f64
#define KEY_BOOL 14
#define KEY_VOID 15
#define KEY_BOX 16
#define KEY_MUT 17

#define KEY_VAR 21
#define KEY_CONST 22
#define KEY_NEW 23
#define KEY_MALLOC 24
#define KEY_FREE 25

#define KEY_IF 31
#define KEY_ELSE 32
#define KEY_SWITCH 33
#define KEY_CASE 34
#define KEY_DEFAULT 35
#define KEY_WHILE 36
#define KEY_FOR 37
#define KEY_BREAK 38
#define KEY_CONTINUE 39
#define KEY_DEFER 40

#define KEY_FUNC 41
#define KEY_RETURN 42
#define KEY_STRUCT 43
#define KEY_TRY 44
#define KEY_CATCH 45
#define KEY_RAISE 46
#define KEY_INCLUDE 47

// token types : operators
#define OP_ADD 51
#define OP_SUB 52
#define OP_MUL_DEREF 53
#define OP_DIV 54
#define OP_MOD 55

#define OP_LT 61
#define OP_LE 62
#define OP_GT 63
#define OP_GE 64
#define OP_EQ 65
#define OP_NE 66

#define OP_AND 67
#define OP_OR 68
#define OP_NOT 69

#define OP_BIT_AND_REF 71
#define OP_BIT_OR 72
#define OP_BIT_NOT 73
#define OP_BIT_XOR 74
#define OP_BIT_LSHIFT 75
#define OP_BIT_RSHIFT 76

// token types : punctuation
#define PUNC_ASSIGN 81
#define PUNC_NULLABLE 82
#define PUNC_SHARP 83

#define PUNC_SEMICOLON 84
#define PUNC_COLON 85
#define PUNC_COMMA 86
#define PUNC_DOT 87

#define PUNC_LPAREN 91
#define PUNC_RPAREN 92
#define PUNC_LBRACE 93
#define PUNC_RBRACE 94
#define PUNC_LBRACKET 95
#define PUNC_RBRACKET 96

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
void TokenVec_init(TokenVec* arr) {
    arr->tokens = malloc(sizeof(Token) * 16);
    arr->count = 0;
    arr->capacity = 16;
}

// TokenVec.add
void TokenVec_add(TokenVec* arr, int line, int type, char* value) {
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
            if (endToken) {
				buffer[bufferIndex] = '\0';
                if (bufferIndex != 0) {
                    bufferIndex = 0;
                    int type = isNumLiteral(buffer);
                    if (type) {
                        TokenVec_add(&tokens, line, type, buffer);
                    }
                    else {
                        type = isKeyword(buffer);
                        if (type) {
                            TokenVec_add(&tokens, line, type, buffer);
                        }
                        else {
                            type = isNameID(buffer);
                            if (type) {
                                TokenVec_add(&tokens, line, type, buffer);
                            }
                            else {
                                printf("E0102 invalid id: %s at %s; %d\n", buffer, filename, line);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
				}
            }
			if (endToken == 2) { // punctuation or operator
				int type = isSingleOper(c);
				if (type) { // single operation
                    buffer[0] = c;
                    buffer[1] = '\0';
                    TokenVec_add(&tokens, line, type, buffer);
                } else if (c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|' || c == '/') { // start of 2-letter operator
                    buffer[bufferIndex++] = c;
                    mode = 1;
                } else {
                    buffer[0] = c;
                    buffer[1] = '\0';
                    printf("E0103 invalid operation: %s at %s; %d\n", buffer, filename, line);
                    exit(EXIT_FAILURE);
                }
            }
            break;

        case 1: // punctuation or operator
            if (c < 128 && !isalnum(c) && !isspace(c)) { // normal punctuation
                if (buffer[0] == '/' && c == '/') { // start of short comment
                    bufferIndex = 0;
                    mode = 2;
                } else if (buffer[0] == '/' && c == '*') { // start of long comment
                    bufferIndex = 0;
                    mode = 3;
                } else {
                    int type = isSingleOper(c);
                    if (type) { // two 1-letter operations
                        bufferIndex = 0;
                        mode = 0;
                        buffer[1] = '\0';
                        TokenVec_add(&tokens, line, isOperPunc(buffer), buffer);
                        buffer[0] = c;
                        TokenVec_add(&tokens, line, isOperPunc(buffer), buffer);
                    } else {
                        buffer[bufferIndex++] = c;
                        buffer[bufferIndex++] = '\0';
                        type = isOperPunc(buffer);
                        if (type) { // 2-letter operation
                            TokenVec_add(&tokens, line, type, buffer);
                            bufferIndex = 0;
                            mode = 0;
                        } else if (c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|' || c == '/') { // 1-letter + maybe 2
                            buffer[1] = '\0';
                            TokenVec_add(&tokens, line, isOperPunc(buffer), buffer);
                            bufferIndex = 0;
                            buffer[bufferIndex++] = c;
                        } else if (c == '\'') { // start of char literal
                            buffer[1] = '\0';
                            TokenVec_add(&tokens, line, isOperPunc(buffer), buffer);
                            bufferIndex = 0;
                            mode = 4;
                        } else if (c == '"') { // start of string literal
                            buffer[1] = '\0';
                            TokenVec_add(&tokens, line, isOperPunc(buffer), buffer);
                            bufferIndex = 0;
                            mode = 6;
                        } else { // invalid
                            printf("E0103 invalid operation: %s at %s; %d\n", buffer, filename, line);
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            } else { // end of punctuation
                buffer[bufferIndex] = '\0';
                int type = isOperPunc(buffer);
                if (type) {
                    TokenVec_add(&tokens, line, type, buffer);
                } else {
                    printf("E0103 invalid operation: %s at %s; %d\n", buffer, filename, line);
                    exit(EXIT_FAILURE);
                }
                bufferIndex = 0;
                mode = 0;
                if (!isspace(c)) {
                    buffer[bufferIndex++] = c;
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
            } else if (c == '\n') { // unterminated string literal
                printf("E0105 unterminated string literal at %s; %d\n", filename, line);
                exit(EXIT_FAILURE);
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

// check numeric literal
int isNumLiteral(char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
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
int isNameID(char* str) {
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
int isKeyword(char* str) {
    if (str == NULL) return 0;
    if (strcmp(str, "u8") == 0 || strcmp(str, "u16") == 0 || strcmp(str, "u32") == 0 || strcmp(str, "u64") == 0 || strcmp(str, "uint") == 0) return KEY_U;
    if (strcmp(str, "i8") == 0 || strcmp(str, "i16") == 0 || strcmp(str, "i32") == 0 || strcmp(str, "i64") == 0 || strcmp(str, "int") == 0) return KEY_I;
    if (strcmp(str, "f32") == 0 || strcmp(str, "f64") == 0) return KEY_F;
    if (strcmp(str, "bool") == 0) return KEY_BOOL;
    if (strcmp(str, "void") == 0) return KEY_VOID;
    if (strcmp(str, "box") == 0) return KEY_BOX;
    if (strcmp(str, "mut") == 0) return KEY_MUT;
    if (strcmp(str, "var") == 0) return KEY_VAR;
    if (strcmp(str, "const") == 0) return KEY_CONST;
    if (strcmp(str, "new") == 0) return KEY_NEW;
    if (strcmp(str, "malloc") == 0) return KEY_MALLOC;
    if (strcmp(str, "free") == 0) return KEY_FREE;
    if (strcmp(str, "if") == 0) return KEY_IF;
    if (strcmp(str, "else") == 0) return KEY_ELSE;
    if (strcmp(str, "switch") == 0) return KEY_SWITCH;
    if (strcmp(str, "case") == 0) return KEY_CASE;
    if (strcmp(str, "default") == 0) return KEY_DEFAULT;
    if (strcmp(str, "while") == 0) return KEY_WHILE;
    if (strcmp(str, "for") == 0) return KEY_FOR;
    if (strcmp(str, "break") == 0) return KEY_BREAK;
    if (strcmp(str, "continue") == 0) return KEY_CONTINUE;
    if (strcmp(str, "defer") == 0) return KEY_DEFER;
    if (strcmp(str, "func") == 0) return KEY_FUNC;
    if (strcmp(str, "return") == 0) return KEY_RETURN;
    if (strcmp(str, "struct") == 0) return KEY_STRUCT;
    if (strcmp(str, "try") == 0) return KEY_TRY;
    if (strcmp(str, "catch") == 0) return KEY_CATCH;
    if (strcmp(str, "raise") == 0) return KEY_RAISE;
    if (strcmp(str, "include") == 0) return KEY_INCLUDE;
    return 0; // not a keyword
}

// check operator or punctuation
int isOperPunc(char* str) {
    if (str == NULL || *str == '\0') return 0;
    if (strcmp(str, "+") == 0) return OP_ADD;
    if (strcmp(str, "-") == 0) return OP_SUB;
    if (strcmp(str, "*") == 0) return OP_MUL_DEREF;
    if (strcmp(str, "/") == 0) return OP_DIV;
    if (strcmp(str, "%") == 0) return OP_MOD;
    if (strcmp(str, "<") == 0) return OP_LT;
    if (strcmp(str, "<=") == 0) return OP_LE;
    if (strcmp(str, ">") == 0) return OP_GT;
    if (strcmp(str, ">=") == 0) return OP_GE;
    if (strcmp(str, "==") == 0) return OP_EQ;
    if (strcmp(str, "!=") == 0) return OP_NE;
    if (strcmp(str, "&&") == 0) return OP_AND;
    if (strcmp(str, "||") == 0) return OP_OR;
    if (strcmp(str, "!") == 0) return OP_NOT;
    if (strcmp(str, "&") == 0) return OP_BIT_AND_REF;
    if (strcmp(str, "|") == 0) return OP_BIT_OR;
    if (strcmp(str, "~") == 0) return OP_BIT_NOT;
    if (strcmp(str, "^") == 0) return OP_BIT_XOR;
    if (strcmp(str, "<<") == 0) return OP_BIT_LSHIFT;
    if (strcmp(str, ">>") == 0) return OP_BIT_RSHIFT;
    if (strcmp(str, "=") == 0) return PUNC_ASSIGN;
    if (strcmp(str, "?") == 0) return PUNC_NULLABLE;
    if (strcmp(str, "#") == 0) return PUNC_SHARP;
    if (strcmp(str, ";") == 0) return PUNC_SEMICOLON;
    if (strcmp(str, ":") == 0) return PUNC_COLON;
    if (strcmp(str, ",") == 0) return PUNC_COMMA;
    if (strcmp(str, ".") == 0) return PUNC_DOT;
    if (strcmp(str, "(") == 0) return PUNC_LPAREN;
    if (strcmp(str, ")") == 0) return PUNC_RPAREN;
    if (strcmp(str, "{") == 0) return PUNC_LBRACE;
    if (strcmp(str, "}") == 0) return PUNC_RBRACE;
    if (strcmp(str, "[") == 0) return PUNC_LBRACKET;
    if (strcmp(str, "]") == 0) return PUNC_RBRACKET;
    return 0; // not an punctuation or operator
}

// check single operation
int isSingleOper(int c) {
    if (c == '+') return OP_ADD;
    if (c == '-') return OP_SUB;
    if (c == '*') return OP_MUL_DEREF;
    if (c == '%') return OP_MOD;
    if (c == '~') return OP_BIT_NOT;
    if (c == '^') return OP_BIT_XOR;
    if (c == '?') return PUNC_NULLABLE;
    if (c == '#') return PUNC_SHARP;
    if (c == ';') return PUNC_SEMICOLON;
    if (c == ':') return PUNC_COLON;
    if (c == ',') return PUNC_COMMA;
    if (c == '.') return PUNC_DOT;
    if (c == '(') return PUNC_LPAREN;
    if (c == ')') return PUNC_RPAREN;
    if (c == '{') return PUNC_LBRACE;
    if (c == '}') return PUNC_RBRACE;
    if (c == '[') return PUNC_LBRACKET;
    if (c == ']') return PUNC_RBRACKET;
    return 0; // not a single operation
}