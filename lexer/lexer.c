#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 256

typedef enum {
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_EQUALS,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_CONFIG,
    TOKEN_KEYWORD,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

const char *keywords[] = {"FUNC", "ADP", "STR", "ART", "OUTLOG", "OUTWAR", "OUTDEB", "OUTERR", "VAR", "CONFIG"};

void error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

void advance(const char **src) {
    (*src)++;
}

void skip_whitespace(const char **src) {
    while (**src && isspace(**src)) {
        advance(src);
    }
}

int is_keyword(const char *identifier) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(identifier, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

Token get_number(const char **src) {
    Token token;
    token.type = TOKEN_NUMBER;
    int length = 0;
    while (**src && isdigit(**src)) {
        token.value[length++] = **src;
        advance(src);
    }
    token.value[length] = '\0';
    return token;
}

Token get_identifier(const char **src) {
    Token token;
    token.type = TOKEN_IDENTIFIER;
    int length = 0;
    while (**src && (isalnum(**src) || **src == '_')) {
        token.value[length++] = **src;
        advance(src);
    }
    token.value[length] = '\0';
    if (is_keyword(token.value)) {
        token.type = TOKEN_KEYWORD;
    }
    return token;
}

Token get_string(const char **src) {
    Token token;
    token.type = TOKEN_STRING;
    int length = 0;
    advance(src);  // Skip opening quote
    while (**src && **src != '"') {
        token.value[length++] = **src;
        advance(src);
    }
    if (**src == '"') {
        advance(src);  // Skip closing quote
    } else {
        error("Unterminated string literal");
    }
    token.value[length] = '\0';
    return token;
}

Token get_config(const char **src) {
    Token token;
    token.type = TOKEN_CONFIG;
    int length = 0;
    while (**src && **src != '\n') {
        token.value[length++] = **src;
        advance(src);
    }
    token.value[length] = '\0';
    return token;
}

Token get_token(const char **src) {
    skip_whitespace(src);

    Token token;
    if (**src == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.value, "EOF");
        return token;
    }
    if (isdigit(**src)) {
        return get_number(src);
    }
    if (isalpha(**src) || **src == '_') {
        return get_identifier(src);
    }
    if (**src == '"') {
        return get_string(src);
    }
    if (**src == '_') {
        if (strncmp(*src, "_CONFIG", 7) == 0) {
            return get_config(src);
        }
    }
    switch (**src) {
        case '+': 
        case '-': 
        case '*': 
        case '/':
            token.type = TOKEN_OPERATOR;
            snprintf(token.value, MAX_TOKEN_LENGTH, "%c", **src);
            advance(src);
            return token;
        case '=': token.type = TOKEN_EQUALS; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case '\n':
            advance(src);
            return get_token(src);  // Recurse to handle new line
        default:
            token.type = TOKEN_INVALID;
            snprintf(token.value, MAX_TOKEN_LENGTH, "Invalid character '%c'", **src);
            return token;
    }
    snprintf(token.value, MAX_TOKEN_LENGTH, "%c", **src);
    advance(src);
    return token;
}

void print_token(Token token) {
    switch (token.type) {
        case TOKEN_NUMBER:
            printf("NUMBER(%s)\n", token.value);
            break;
        case TOKEN_IDENTIFIER:
        case TOKEN_KEYWORD:
            printf("IDENTIFIER(%s)\n", token.value);
            break;
        case TOKEN_STRING:
            printf("STRING(%s)\n", token.value);
            break;
        case TOKEN_OPERATOR:
            printf("OPERATOR(%s)\n", token.value);
            break;
        case TOKEN_EQUALS:
            printf("EQUALS\n");
            break;
        case TOKEN_SEMICOLON:
            printf("SEMICOLON\n");
            break;
        case TOKEN_LPAREN:
            printf("LPAREN\n");
            break;
        case TOKEN_RPAREN:
            printf("RPAREN\n");
            break;
        case TOKEN_CONFIG:
            printf("CONFIG(%s)\n", token.value);
            break;
        case TOKEN_EOF:
            printf("EOF\n");
            break;
        case TOKEN_INVALID:
            printf("INVALID(%s)\n", token.value);
            break;
    }
}

int main() {
    const char *source_code = "_CONFIG-349:\n_ART-ORD = \"BODMAS\"\nADP myVar = 6;\nFUNC myFunc(x, y) { OUTLOG \"Sum is: \" + x + y; }";
    const char *src = source_code;
    Token token;

    while ((token = get_token(&src)).type != TOKEN_EOF) {
        print_token(token);
    }
    print_token(token);  // Print EOF token

    return 0;
}
