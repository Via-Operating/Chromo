#include <stdio.h>
#include <ctype.h>
#include <string.h>

// Token types
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_COLON,
    TOKEN_BRACE_OPEN,
    TOKEN_BRACE_CLOSE,
    TOKEN_SEMICOLON, // Never gonna be used (yet) :(
    TOKEN_ASSIGN,
    TOKEN_SUBCLUSIVE,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[256];
    int line;
    int column;
} Token;

typedef struct {
    const char *source;
    size_t length;
    size_t index;
    int line;
    int column;
} Lexer;

void initLexer(Lexer *lexer, const char *source);
Token nextToken(Lexer *lexer);
void printToken(Token token);

void initLexer(Lexer *lexer, const char *source) {
    lexer->source = source;
    lexer->length = strlen(source);
    lexer->index = 0;
    lexer->line = 1;
    lexer->column = 1;
}

char currentChar(Lexer *lexer) {
    return lexer->index < lexer->length ? lexer->source[lexer->index] : '\0';
}

void advance(Lexer *lexer) {
    if (currentChar(lexer) == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->index++;
}

void skipWhitespace(Lexer *lexer) {
    while (isspace(currentChar(lexer))) {
        advance(lexer);
    }
}

Token nextToken(Lexer *lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    token.text[0] = '\0';

    skipWhitespace(lexer);

    char c = currentChar(lexer);

    if (c == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }

    if (isalpha(c) || c == '_') {
        size_t start = lexer->index;
        while (isalnum(currentChar(lexer)) || currentChar(lexer) == '_') {
            advance(lexer);
        }
        strncpy(token.text, lexer->source + start, lexer->index - start);
        token.text[lexer->index - start] = '\0';
        token.type = TOKEN_IDENTIFIER;
        return token;
    }

    if (isdigit(c)) {
        size_t start = lexer->index;
        while (isdigit(currentChar(lexer))) {
            advance(lexer);
        }
        strncpy(token.text, lexer->source + start, lexer->index - start);
        token.text[lexer->index - start] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    switch (c) {
        case ':':
            token.type = TOKEN_COLON;
            token.text[0] = ':';
            token.text[1] = '\0';
            advance(lexer);
            break;
        case '{':
            token.type = TOKEN_BRACE_OPEN;
            token.text[0] = '{';
            token.text[1] = '\0';
            advance(lexer);
            break;
        case '}':
            token.type = TOKEN_BRACE_CLOSE;
            token.text[0] = '}';
            token.text[1] = '\0';
            advance(lexer);
            break;
        case ';':
            token.type = TOKEN_SEMICOLON;
            token.text[0] = ';';
            token.text[1] = '\0';
            advance(lexer);
            break;
        case '=':
            token.type = TOKEN_ASSIGN;
            token.text[0] = '=';
            token.text[1] = '\0';
            advance(lexer);
            break;
        case '#':
            size_t start; start = lexer->index;
            advance(lexer);
            if (currentChar(lexer) == 'v') {
                advance(lexer);
                if (strncmp(lexer->source + start, "#var", 4) == 0) {
                    token.type = TOKEN_IDENTIFIER;
                    strncpy(token.text, "#var", 4);
                    token.text[4] = '\0';
                } else {
                    token.type = TOKEN_UNKNOWN;
                    strncpy(token.text, lexer->source + start, lexer->index - start);
                    token.text[lexer->index - start] = '\0';
                }
            } else {
                token.type = TOKEN_UNKNOWN;
                strncpy(token.text, lexer->source + start, lexer->index - start);
                token.text[lexer->index - start] = '\0';
            }
            break;
        default:
            token.type = TOKEN_UNKNOWN;
            token.text[0] = c;
            token.text[1] = '\0';
            advance(lexer);
            break;
    }

    return token;
}

void printToken(Token token) {
    const char *typeNames[] = {
        "IDENTIFIER", "NUMBER", "COLON", "BRACE_OPEN", "BRACE_CLOSE", "SEMICOLON", "ASSIGN", "SUBCLUSIVE", "UNKNOWN", "EOF"
    };
    printf("Token: %d, Type: %s, Text: '%s', Line: %d, Column: %d\n",
           token.type, typeNames[token.type], token.text, token.line, token.column);
}

int main() {
    const char *code = "#var:struct MyStruct { int x; int y; x = 10; y = 20; } subclusive:#";
    Lexer lexer;
    initLexer(&lexer, code);

    Token token;
    do {
        token = nextToken(&lexer);
        printToken(token);
    } while (token.type != TOKEN_EOF);

    return 0;
}
