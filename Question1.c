#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main() {
    pid_t pid;
    int status;

    // Temporary file setup
    char *temp_filename = "/tmp/my_tempfile.txt";
    int fd = open(temp_filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to create temporary file");
        return 1;
    }

    // Write to the temp file
    const char *message = "Temporary file written by parent process.\n";
    write(fd, message, strlen(message));
    close(fd);

    // Fork a child process
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // In child: Execute command using execvp()
        char *args[] = {"ls", "-l", "/tmp", NULL};
        execvp(args[0], args);

        // If execvp fails
        perror("execvp failed");
        exit(1);
    } else {
        // In parent: Wait for the child to complete
        wait(&status);
        printf("\nChild process finished.\n");

        // Read from the temp file
        fd = open(temp_filename, O_RDONLY);
        if (fd < 0) {
            perror("Failed to open temp file for reading");
            return 1;
        }

        char buffer[256];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Parent read from file:\n%s\n", buffer);
        }

        close(fd);
        unlink(temp_filename); // Clean up
    }

    return 0;
}
