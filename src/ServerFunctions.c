#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Server.h"

#define MAX_SIZE 10

int identifyIPVersion(const char *vrs) {
    if(strcmp(vrs, "v4") == 0) return 1;
    else if (strcmp(vrs, "v6") == 0) return 2;
    else return 0;
}

int** getMazeFromFile(const char* filename, int* size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fclose(file);
        DieWithUserMessage("File not open","Please check if file path is correct");
        return NULL;
    }

    // Auxiliary variables to determine the size of the matrix
    char line[1024]; // Buffer to read a line
    int count = 0;

    // Determine the size of the matrix by counting the elements in the first line
    if (fgets(line, sizeof(line), file) != NULL) {
        char* token = strtok(line, " ");
        while (token != NULL) {
            count++;
            token = strtok(NULL, " ");
        }
    }

    // Restart the file to read the complete matrix
    rewind(file);

    // Validate the matrix size (must be 5 or 10)
    if (count != 5 && count != 10) {
        fprintf(stderr, "Error: Matrix invalid. Must be a 5x5 or 10x10 matrix.\n");
        fclose(file);
        return NULL;
    }

    *size = count;

    // Allocate memory for the matrix
    int** matrix = (int**)malloc(*size * sizeof(int*));
    for (int i = 0; i < *size; i++) {
        matrix[i] = (int*)malloc(*size * sizeof(int));
    }

    // Fill the matrix with values from the file
    for (int i = 0; i < *size; i++) {
        for (int j = 0; j < *size; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                fprintf(stderr, "Error on reading values of matrix.\n");
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}
