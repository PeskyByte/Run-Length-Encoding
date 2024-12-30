#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    char* data;
    int start;
    int end;
    char* result;
    int* result_len;
    int offset;
} ThreadData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* encode_chunk(void* arg) {
    ThreadData* td = (ThreadData*)arg;
    int count = 1;
    char current;
    int pos = 0;
    char* temp = malloc(2 * (td->end - td->start) + 1);
    
    current = td->data[td->start];
    
    for (int i = td->start + 1; i < td->end; i++) {
        if (td->data[i] == current) {
            count++;
        } else {
            pos += sprintf(temp + pos, "%d%c", count, current);
            current = td->data[i];
            count = 1;
        }
    }
    
    pos += sprintf(temp + pos, "%d%c", count, current);
    
    pthread_mutex_lock(&mutex);
    memcpy(td->result + td->offset, temp, pos);
    *td->result_len += pos;
    pthread_mutex_unlock(&mutex);
    
    free(temp);
    return NULL;
}

int main(int argc, char* argv[]) {
    clock_t begin = clock();
    
    if (argc != 4) {
        printf("Usage: %s <input_file> <output_file> <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[3]);
    if (num_threads <= 0) {
        printf("Number of threads must be positive\n");
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    
    if (!input || !output) {
        printf("Error opening files\n");
        return 1;
    }

    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    if (file_size == 0) {
        printf("Input file is empty\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    char* data = malloc(file_size + 1);
    if (!data) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    size_t bytes_read = fread(data, 1, file_size, input);
    data[bytes_read] = '\0';

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = malloc(num_threads * sizeof(ThreadData));
    char* result = malloc(file_size * 2 + 1);
    
    if (!threads || !thread_data || !result) {
        printf("Memory allocation failed\n");
        free(data);
        free(threads);
        free(thread_data);
        free(result);
        fclose(input);
        fclose(output);
        return 1;
    }

    int result_len = 0;
    int chunk_size = file_size / num_threads;
    int offset = 0;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].data = data;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? file_size : (i + 1) * chunk_size;
        thread_data[i].result = result;
        thread_data[i].result_len = &result_len;
        thread_data[i].offset = offset;
        offset += (thread_data[i].end - thread_data[i].start) * 2;
        
        if (pthread_create(&threads[i], NULL, encode_chunk, &thread_data[i]) != 0) {
            printf("Thread creation failed\n");
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            free(data);
            free(threads);
            free(thread_data);
            free(result);
            fclose(input);
            fclose(output);
            return 1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    result[result_len] = '\0';
    fwrite(result, 1, result_len, output);

    free(data);
    free(threads);
    free(thread_data);
    free(result);
    fclose(input);
    fclose(output);
    pthread_mutex_destroy(&mutex);
    
    clock_t time = clock() - begin;
    printf("Total time: %f seconds\n", (double)time / CLOCKS_PER_SEC);
    
    return 0;
}