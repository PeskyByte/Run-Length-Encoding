#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
void encode_chunk(char* data, int size, char* result, int* result_size) {
    int count = 1;
    char current;
    *result_size = 0;
    
    for (int i = 0; i < size - 1; i++) {
        current = data[i];
        if (data[i + 1] == current) {
            count++;
        } else {
            *result_size += sprintf(result + *result_size, "%d%c", count, current);
            count = 1;
        }
    }
    
    if (size > 0) {
        *result_size += sprintf(result + *result_size, "%d%c", count, data[size - 1]);
    }
}

int main(int argc, char* argv[]) {
    double begin;
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    begin = MPI_Wtime();
    if (argc != 3) {
        if (rank == 0)
            printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    long file_size;
    char *data = NULL;
    int chunk_size;
    int local_size;
    char *local_data;
    
    if (rank == 0) {
        
        FILE* input = fopen(argv[1], "r");
        if (!input) {
            printf("Error opening input file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(input, 0, SEEK_END);
        file_size = ftell(input);
        fseek(input, 0, SEEK_SET);

        data = malloc(file_size);
        fread(data, 1, file_size, input);
        fclose(input);
    }

    MPI_Bcast(&file_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    chunk_size = file_size / size;
    local_size = (rank == size - 1) ? (file_size - rank * chunk_size) : chunk_size;
    local_data = malloc(local_size);

    MPI_Scatter(data, chunk_size, MPI_CHAR,
                local_data, chunk_size, MPI_CHAR,
                0, MPI_COMM_WORLD);

    char* local_result = malloc(local_size * 2);
    int local_result_size;
    encode_chunk(local_data, local_size, local_result, &local_result_size);

    int* result_sizes = NULL;
    int* displacements = NULL;
    if (rank == 0) {
        result_sizes = malloc(size * sizeof(int));
        displacements = malloc(size * sizeof(int));
    }

    MPI_Gather(&local_result_size, 1, MPI_INT,
               result_sizes, 1, MPI_INT,
               0, MPI_COMM_WORLD);

    int total_size = 0;
    if (rank == 0) {
        displacements[0] = 0;
        for (int i = 0; i < size; i++) {
            if (i > 0)
                displacements[i] = displacements[i-1] + result_sizes[i-1];
            total_size += result_sizes[i];
        }
    }

    char* final_result = NULL;
    if (rank == 0)
        final_result = malloc(total_size);

    MPI_Gatherv(local_result, local_result_size, MPI_CHAR,
                final_result, result_sizes, displacements, MPI_CHAR,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        FILE* output = fopen(argv[2], "w");
        if (!output) {
            printf("Error opening output file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fwrite(final_result, 1, total_size, output);
        fclose(output);

        free(data);
        free(result_sizes);
        free(displacements);
        free(final_result);
    }
    free(local_data);
    free(local_result);
    if (rank == 0) {
        
        double time = MPI_Wtime() - begin;
        printf("Total time: %f seconds\n", time);
    }
    MPI_Finalize();
    return 0;
}