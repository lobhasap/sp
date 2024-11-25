#include <stdio.h>
#include <string.h>

// Define structures for MOT and Symbol Table
struct MOTEntry {
    char mnemonic[10];
    char opcode[5];
};

struct SymbolEntry {
    char symbol[10];
    int address;
};

// Hardcoded MOT
struct MOTEntry MOT[] = {
    {"MOV", "01"},
    {"ADD", "02"},
    {"SUB", "03"},
    {"MUL", "04"},
    {"DIV", "05"}
};
int MOT_SIZE = 5;

// Hardcoded Symbol Table
struct SymbolEntry ST[] = {
    {"A", 100},
    {"B", 101},
    {"C", 102}
};
int ST_SIZE = 3;

// Hardcoded Intermediate Code
char IC[][20] = {
    "(IS,01) A B",
    "(IS,02) A C",
    "(IS,03) C B"
};
int IC_SIZE = 3;

// Function to extract the opcode from Intermediate Code
char* extractOpcode(char* instrType) {
    static char opcode[5];
    if (sscanf(instrType, "IS,%s", opcode) == 1) {
        return opcode;
    }
    return NULL;
}

// Function to get the address from the Symbol Table
int getSymbolAddress(char* symbol) {
    for (int i = 0; i < ST_SIZE; i++) {
        if (strcmp(ST[i].symbol, symbol) == 0) {
            return ST[i].address;
        }
    }
    return -1; // Symbol not found
}

// Main function to generate machine code
int main() {
    printf("Machine Code Output:\n");
    for (int i = 0; i < IC_SIZE; i++) {
        char instrType[20], operand1[10], operand2[10];
        int res = sscanf(IC[i], "(%[^)]) %s %s", instrType, operand1, operand2);

        if (res >= 2) {
            // Extract opcode from instruction type
            char* opcode = extractOpcode(instrType);
            if (opcode == NULL) {
                printf("Error: Invalid instruction type in %s\n", IC[i]);
                continue;
            }

            // Get symbol addresses
            int addr1 = getSymbolAddress(operand1);
            int addr2 = (res == 3) ? getSymbolAddress(operand2) : -1;

            // Validate addresses and print machine code
            if (addr1 != -1 && (addr2 != -1 || res == 2)) {
                printf("%s %d %d\n", opcode, addr1, addr2);
            } else {
                printf("Error: Undefined symbol in %s\n", IC[i]);
            }
        } else {
            printf("Error: Invalid intermediate code format in %s\n", IC[i]);
        }
    }

    return 0;
}
