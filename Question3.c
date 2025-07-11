#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_NUMBER 100000
#define THREAD_COUNT 20

// Global count of palindromic numbers
int total_palindromes = 0;
pthread_mutex_t mutex;

// Function to check if a number is a palindrome
bool is_palindrome(int num) {
    int original = num;
    int reversed = 0;
    while (num > 0) {
        reversed = reversed * 10 + (num % 10);
        num /= 10;
    }
    return original == reversed;
}

// Struct to hold the start and end range for each thread
typedef struct {
    int start;
    int end;
} ThreadRange;

// Thread function
void* count_palindromes(void* arg) {
    ThreadRange* range = (ThreadRange*)arg;
    int local_count = 0;

    for (int i = range->start; i <= range->end; i++) {
        if (is_palindrome(i)) {
            local_count++;
        }
    }

    // Synchronize update to global count
    pthread_mutex_lock(&mutex);
    total_palindromes += local_count;
    pthread_mutex_unlock(&mutex);

    free(range);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    pthread_mutex_init(&mutex, NULL);

    int range_size = MAX_NUMBER / THREAD_COUNT;

    // Create threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        ThreadRange* range = malloc(sizeof(ThreadRange));
        range->start = i * range_size;
        range->end = (i == THREAD_COUNT - 1) ? MAX_NUMBER : (range->start + range_size - 1);

        if (pthread_create(&threads[i], NULL, count_palindromes, range) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    // Output the result
    printf("The Synchronize total number of palindromic number from 0 to 100,000 is %d\n", total_palindromes);
    return 0;
}
