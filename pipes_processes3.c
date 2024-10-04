#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grep_argument>\n", argv[0]);
        exit(1);
    }

    int pipe1[2];  // Pipe for cat to grep
    int pipe2[2];  // Pipe for grep to sort
    pid_t pid1, pid2;

    // Create first pipe (for cat to grep)
    if (pipe(pipe1) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create second pipe (for grep to sort)
    if (pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork first child for grep
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        // Child process for grep
        // Replace standard input with input end of pipe1
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[1]); // Close unused write end of pipe1
        close(pipe1[0]); // Close read end of pipe1 after duplicating

        // Prepare grep arguments
        char *grep_args[] = {"grep", argv[1], NULL};

        // Replace standard output with output end of pipe2
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[0]); // Close unused read end of pipe2
        close(pipe2[1]); // Close write end of pipe2 after duplicating

        // Execute grep
        execvp("grep", grep_args);
        perror("execvp grep"); // If execvp fails
        exit(1);
    }

    // Fork second child for sort
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0) {
        // Child process for sort
        // Replace standard input with input end of pipe2
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[1]); // Close unused write end of pipe2
        close(pipe2[0]); // Close read end of pipe2 after duplicating

        // Execute sort
        char *sort_args[] = {"sort", NULL};
        execvp("sort", sort_args);
        perror("execvp sort"); // If execvp fails
        exit(1);
    }

    // Parent process for cat
    // Replace standard output with output end of pipe1
    dup2(pipe1[1], STDOUT_FILENO);
    close(pipe1[0]); // Close unused read end of pipe1
    close(pipe1[1]); // Close write end of pipe1 after duplicating

    // Prepare cat arguments
    char *cat_args[] = {"cat", "scores", NULL};

    // Execute cat
    execvp("cat", cat_args);
    perror("execvp cat"); // If execvp fails

    return 0;
}
