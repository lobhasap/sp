#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Structure Definitions
typedef struct {
    char macroName[50];
    int mdtIndex;
    int numParams;
} MNTEntry;

typedef struct {
    int index;
    char definition[100];
} MDTEntry;

// Global Variables
char APTAB[10][50];
MNTEntry MNT[10];
MDTEntry MDT[100];
int mntCount = 0;
int mdtCount = 0;

// Function to Initialize Tables
void initializeTables() {
    FILE *macroDefFile = fopen("macrodef.txt", "r");
    if (macroDefFile == NULL) {
        printf("Error: Cannot open macro_def.txt\n");
        exit(1);
    }
    
    char line[100];
    int mdtIndex = 0;
    
    while (fgets(line, sizeof(line), macroDefFile)) {
        if (strstr(line, "MACRO") != NULL) {
            char macroName[50], param[10];
            sscanf(line, "MACRO %s %s", macroName, param);
            
            MNT[mntCount].mdtIndex = mdtIndex;
            MNT[mntCount].numParams = param[0] ? 1 : 0; // Assuming only one parameter
            strcpy(MNT[mntCount].macroName, macroName);
            mntCount++;
            
            strcpy(MDT[mdtCount].definition, line);
            MDT[mdtCount].index = mdtCount;
            mdtCount++;
            
            while (fgets(line, sizeof(line), macroDefFile) && strstr(line, "MEND") == NULL) {
                strcpy(MDT[mdtCount].definition, line);
                MDT[mdtCount].index = mdtCount;
                mdtCount++;
            }
            
            strcpy(MDT[mdtCount].definition, "MEND");
            MDT[mdtCount].index = mdtCount;
            mdtCount++;
        }
    }
    
    fclose(macroDefFile);
}

// Function to Display Tables
void displayTables() {
    printf("\nMacro Name Table (MNT):\n");
    printf("Macro Name\tMDT Index\tNumber of Parameters\n");
    for (int i = 0; i < mntCount; i++) {
        printf("%s\t\t%d\t\t%d\n", MNT[i].macroName, MNT[i].mdtIndex, MNT[i].numParams);
    }
    
    printf("\nMacro Definition Table (MDT):\n");
    printf("Index\tDefinition\n");
    for (int i = 0; i < mdtCount; i++) {
        printf("%d\t%s", MDT[i].index, MDT[i].definition);
    }
}

// Function to Expand a Macro
void expandMacro(const char *macroName, const char *actualParam) {
    int found = 0;
    int mntIndex;
    
    for (int i = 0; i < mntCount; i++) {
        if (strcmp(MNT[i].macroName, macroName) == 0) {
            found = 1;
            mntIndex = i;
            break;
        }
    }
    
    if (!found) {
        printf("Error: Macro %s not found!\n", macroName);
        return;
    }
    
    strcpy(APTAB[0], actualParam);
    printf("\nExpanded Code for %s:\n", macroName);
    
    for (int i = MNT[mntIndex].mdtIndex + 1; i < mdtCount; i++) {
        if (strstr(MDT[i].definition, "MEND") != NULL) break;
        
        char expandedLine[100];
        strcpy(expandedLine, MDT[i].definition);
        char *paramPosition = strstr(expandedLine, "&ARG1");
        
        if (paramPosition != NULL) {
            strncpy(paramPosition, APTAB[0], strlen(APTAB[0]));
            paramPosition[strlen(APTAB[0])] = '\0';
        }
        
        printf("%s", expandedLine);
    }
    
    printf("\nActual Parameter Table (APTAB):\n");
    printf("Parameter\tValue\n");
    printf("&ARG1\t\t%s\n", APTAB[0]);
}

// Function to Process Macro Calls
void processMacroCalls() {
    FILE *macroCallsFile = fopen("macrocal.txt", "r");
    if (macroCallsFile == NULL) {
        printf("Error: Cannot open macrocal.txt\n");
        exit(1);
    }
    
    char line[100];
    
    while (fgets(line, sizeof(line), macroCallsFile)) {
        char macroName[50], actualParam[50];
        sscanf(line, "%s %s", macroName, actualParam);
        expandMacro(macroName, actualParam);
    }
    
    displayTables();
    fclose(macroCallsFile);
}

// Main Function
int main() {
    initializeTables();
    processMacroCalls();
    printf("Macro expansion completed. Output displayed in terminal.\n");
    return 0;
}
