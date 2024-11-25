#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEXEMES 100
#define MAX_LINE_NUMBERS 200
#define MAX_SYMBOLS 200

struct Lexeme {
    int lineno;
    char lexeme[10];
    char token[10];
    char tokenvalue[10];
};

struct Symbol {
    int index;
    char symbol[10];
};

const char *keywords[] = {"int", "char", "const", "if", "else", "for", "while", "do"};
int keywordcount = 8;
const char *operators[] = {"+", "-", "++", "--", "<", ">", "<=", ">=","="};
int operatorcount = 9;
const char *delimeters[] = {"(", ")", "{", "}", ":", ";"};
int delimetercount = 6;

struct Lexeme lexemetable[MAX_LEXEMES];
int lexemecount = 0;
struct Symbol symboltable[MAX_SYMBOLS];
int symbolcount = 0;

int is_keyword(const char *str) {
    for (int i = 0; i < keywordcount; i++) {
        if (strcmp(keywords[i], str) == 0) {
            return i;
        }
    }
    return -1;
}

int is_operator(const char *str) {
    for (int i = 0; i < operatorcount; i++) {
        if (strcmp(operators[i], str) == 0) {
            return i;
        }
    }
    return -1;
}

int is_delimeter(const char *str) {
    for (int i = 0; i < delimetercount; i++) {
        if (strcmp(delimeters[i], str) == 0) {
            return i;
        }
    }
    return -1;
}

int add_symbol(const char *symbol) {
    for (int i = 0; i < symbolcount; i++) {
        if (strcmp(symboltable[i].symbol, symbol) == 0) {
            return i;
        }
    }
    symboltable[symbolcount].index = symbolcount;
    strcpy(symboltable[symbolcount].symbol, symbol);
    symbolcount++;
    return symbolcount - 1;
}

void add_lexeme(int line_no, const char *lexeme, const char *token, const char *tokenvalue) {
    lexemetable[lexemecount].lineno = line_no;
    strcpy(lexemetable[lexemecount].lexeme, lexeme);
    strcpy(lexemetable[lexemecount].token, token);
    strcpy(lexemetable[lexemecount].tokenvalue, tokenvalue);
    lexemecount++;
}

void lexicalanalyzer(FILE *file) {
    char line[MAX_LINE_NUMBERS];
    int linecount = 0;

    while (fgets(line, sizeof(line), file)) {
        linecount++;
        char *token = strtok(line, " \t\n");

        while (token != NULL) {
            int index;
            if ((index = is_keyword(token)) != -1) {
                char index_str[10];
                sprintf(index_str, "%d", index);
                add_lexeme(linecount, token, "KEYWORD", index_str);
                printf("Line %d: Lexeme = %s, Token = KEYWORD, Value = %s\n", linecount, token, index_str);
            } else if ((index = is_operator(token)) != -1) {
                char index_str[10];
                sprintf(index_str, "%d", index);
                add_lexeme(linecount, token, "OPERATOR", index_str);
                printf("Line %d: Lexeme = %s, Token = OPERATOR, Value = %s\n", linecount, token, index_str);
            } else if ((index = is_delimeter(token)) != -1) {
                char index_str[10];
                sprintf(index_str, "%d", index);
                add_lexeme(linecount, token, "DELIMETER", index_str);
                printf("Line %d: Lexeme = %s, Token = DELIMETER, Value = %s\n", linecount, token, index_str);
            } else {
                int symbol_index = add_symbol(token);
                char index_str[10];
                sprintf(index_str, "%d", symbol_index);
                add_lexeme(linecount, token, "IDENTIFIER", index_str);
                printf("Line %d: Lexeme = %s, Token = IDENTIFIER, Value = %s\n", linecount, token, index_str);
            }
            token = strtok(NULL, " \t\n");
        }
    }
}

int main() {
    FILE *file = fopen("inputc.txt", "r");
    if (file == NULL) {
        printf("Error: Unable to open the input file.\n");
        return 1;
    }

    lexicalanalyzer(file);
    fclose(file);

    printf("\nSymbol Table:\n");
    for (int i = 0; i < symbolcount; i++) {
        printf("Index = %d, Symbol = %s\n", symboltable[i].index, symboltable[i].symbol);
    }

    return 0;
}
