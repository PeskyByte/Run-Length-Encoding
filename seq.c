#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void encode(FILE* input, FILE* output) {
    int count = 1;
    char current, previous;
    
    if (fread(&previous, sizeof(char), 1, input) != 1)
        return;

    while (fread(&current, sizeof(char), 1, input) == 1) {
        if (current == previous) {
            count++;
        } else {
            fprintf(output, "%d%c", count, previous);
            count = 1;
            previous = current;
        }
    }
    fprintf(output, "%d%c", count, previous);
}

int main(int argc, char* argv[]) {
    clock_t begin = clock();
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    
    if (!input || !output) {
        printf("Error opening files\n");
        return 1;
    }

    encode(input, output);

    fclose(input);
    fclose(output);
    clock_t time = clock() - begin;
    printf("Total time: %f seconds\n", (double)time / CLOCKS_PER_SEC);
    return 0;
}