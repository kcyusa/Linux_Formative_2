// buffered_log.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOG_COUNT 1000000

int main() {
    FILE *fp = fopen("buffered_log.txt", "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    for (int i = 0; i < LOG_COUNT; i++) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] Log entry %d\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, i);
    }

    fclose(fp);
    return 0;
}
