#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 4096

typedef struct {
    const char* input_file;
    FILE* output_file;
    pthread_mutex_t* mutex;
} ThreadArgs;

void* merge_log(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    FILE* in = fopen(args->input_file, "r");
    if (!in) {
        perror("Error opening input file");
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        pthread_mutex_lock(args->mutex);
        fwrite(buffer, 1, bytes, args->output_file);
        pthread_mutex_unlock(args->mutex);
    }

    fclose(in);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s merged_output.txt file1.txt file2.txt ... <num_threads>\n", argv[0]);
        return 1;
    }

    int thread_count = atoi(argv[argc - 1]);
    int input_file_count = argc - 3;

    if (thread_count <= 0 || thread_count > input_file_count) {
        fprintf(stderr, "Invalid thread count.\n");
        return 1;
    }

    FILE* output_file = fopen(argv[1], "w");
    if (!output_file) {
        perror("Error opening output file");
        return 1;
    }

    pthread_t threads[thread_count];
    ThreadArgs args[thread_count];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // Assign files to threads round-robin
    int current_file = 0;
    for (int i = 0; i < thread_count; ++i) {
        args[i].input_file = argv[2 + current_file++];
        args[i].output_file = output_file;
        args[i].mutex = &mutex;

        if (pthread_create(&threads[i], NULL, merge_log, &args[i]) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    // Handle any remaining files sequentially with new threads
    while (current_file < input_file_count) {
        ThreadArgs temp_arg = {
            .input_file = argv[2 + current_file++],
            .output_file = output_file,
            .mutex = &mutex
        };
        merge_log(&temp_arg);
    }

    for (int i = 0; i < thread_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    fclose(output_file);
    pthread_mutex_destroy(&mutex);

    printf("Merging completed.\n");
    return 0;
}
