#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char mnemonic[10];
    int type;  // 0: AD, 1: IS, 2: DL
    int opcode;
} MOTEntry;

typedef struct {
    char symbol[10];
    int address;
} SymbolTableEntry;

typedef struct {
    char literal[10];
    int address;
} LiteralTableEntry;

int poolTable[100];
int poolTableSize = 0;

MOTEntry MOT[] = {
    {"START", 0, 1}, {"END", 0, 2}, {"LTORG", 0, 3},
    {"MOV", 1, 1}, {"ADD", 1, 2}, {"SUB", 1, 3},
    {"DC", 2, 1}, {"DS", 2, 2}
};

int motSize = sizeof(MOT) / sizeof(MOT[0]);

SymbolTableEntry symbolTable[100];
int symbolTableSize = 0;

LiteralTableEntry literalTable[100];
int literalTableSize = 0;

int searchMOT(char *mnemonic) {
    for (int i = 0; i < motSize; i++) {
        if (strcmp(MOT[i].mnemonic, mnemonic) == 0)
            return i;
    }
    return -1;
}

void addSymbol(char *symbol, int address) {
    strcpy(symbolTable[symbolTableSize].symbol, symbol);
    symbolTable[symbolTableSize].address = address;
    symbolTableSize++;
}

int searchSymbolTable(char *symbol) {
    for (int i = 0; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i].symbol, symbol) == 0)
            return i;
    }
    return -1;
}

void addLiteral(char *literal) {
    for (int i = 0; i < literalTableSize; i++) {
        if (strcmp(literalTable[i].literal, literal) == 0)
            return;
    }
    strcpy(literalTable[literalTableSize].literal, literal);
    literalTable[literalTableSize].address = -1;  // Address to be assigned later
    literalTableSize++;
}

int isValidRegister(char *operand) {
    return (strcmp(operand, "A") == 0 || strcmp(operand, "B") == 0 ||
            strcmp(operand, "C") == 0 || strcmp(operand, "D") == 0);
}

void processLiterals(int *locationCounter) {
    // Add a new pool start index only if there are unprocessed literals
    if (literalTableSize > 0 && poolTable[poolTableSize - 1] != literalTableSize) {
        poolTable[poolTableSize++] = literalTableSize;
    }

    // Assign addresses to all unprocessed literals
    for (int i = 0; i < literalTableSize; i++) {
        if (literalTable[i].address == -1) {  // Unassigned literal
            literalTable[i].address = (*locationCounter)++;
        }
    }
}



void processALP(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file.\n");
        return;
    }

    char line[50], mnemonic[10], operand1[10], operand2[10];
    int locationCounter = 0, lineNum = 1;

    printf("Intermediate Code:\n");
    printf("Line    Type        Code        Operand1        Operand2\n");

    while (fgets(line, sizeof(line), file)) {
        int numOperands = sscanf(line, "%s %s %s", mnemonic, operand1, operand2);
        int motIndex = searchMOT(mnemonic);

        if (motIndex == -1) {
            printf("Error on line %d: Invalid instruction '%s'\n", lineNum, mnemonic);
            lineNum++;
            continue;
        }

        MOTEntry instruction = MOT[motIndex];
        printf("%-8d", lineNum);

        if (instruction.type == 0) {  // Assembler Directive
            printf("(AD, %02d)   ", instruction.opcode);

            if (strcmp(mnemonic, "START") == 0) {
                locationCounter = atoi(operand1);
                printf("(C, %s)\n", operand1);
            } else if (strcmp(mnemonic, "END") == 0 || strcmp(mnemonic, "LTORG") == 0) {
                printf(" -             -\n");
                processLiterals(&locationCounter);
            }
        } else if (instruction.type == 1) {  // Imperative Statement
            printf("(IS, %02d)   ", instruction.opcode);

            if (isValidRegister(operand1)) {
                printf("(R, %s)       ", operand1);
            } else {
                printf("(S, %s)       ", operand1);
                if (searchSymbolTable(operand1) == -1) {
                    addSymbol(operand1, locationCounter);
                }
            }

            if (numOperands == 3) {
                if (operand2[0] == '=') {
                    printf("(L, %s)\n", operand2 + 1);
                    addLiteral(operand2);
                } else {
                    printf("(C, %s)\n", operand2);
                }
            } else {
                printf("-\n");
            }
            locationCounter++;
        } else if (instruction.type == 2) {  // Declarative Statement
            printf("(DL, %02d)   ", instruction.opcode);
            printf("(S, %s)       (C, %s)\n", operand1, operand2);

            addSymbol(operand1, locationCounter);
            if (strcmp(mnemonic, "DS") == 0) {
                locationCounter += atoi(operand2);
            } else {
                locationCounter++;
            }
        }
        lineNum++;
    }

    fclose(file);

    printf("\nSymbol Table:\n");
    printf("Symbol    Address\n");
    for (int i = 0; i < symbolTableSize; i++) {
        printf("%-10s%-10d\n", symbolTable[i].symbol, symbolTable[i].address);
    }

    printf("\nLiteral Table:\n");
    printf("Literal    Address\n");
    for (int i = 0; i < literalTableSize; i++) {
        printf("%-10s%-10d\n", literalTable[i].literal, literalTable[i].address);
    }

    printf("\nPool Table:\n");
    printf("Pool Start\n");
    for (int i = 0; i < poolTableSize; i++) {
        printf("%-10d\n", poolTable[i]);
    }
}

int main() {
    processALP("ltorg.txt");
    return 0;
}
