// unbuffered_log.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define LOG_COUNT 1000000

int main() {
    int fd = open("unbuffered_log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char buffer[128];
    for (int i = 0; i < LOG_COUNT; i++) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        int len = snprintf(buffer, sizeof(buffer), "[%04d-%02d-%02d %02d:%02d:%02d] Log entry %d\n",
                           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                           t->tm_hour, t->tm_min, t->tm_sec, i);
        write(fd, buffer, len);
    }

    close(fd);
    return 0;
}
