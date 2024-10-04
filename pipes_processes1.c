// C program to demonstrate use of fork() and pipe()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    // We use two pipes
    // First pipe to send input string from parent
    // Second pipe to send concatenated string from child

    int fd1[2];  // Pipe to send data from parent to child
    int fd2[2];  // Pipe to send data from child to parent

    char fixed_str[] = "howard.edu";
    char input_str[100];
    pid_t p;

    // Create the first pipe
    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe 1 Failed" );
        return 1;
    }

    // Create the second pipe
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe 2 Failed" );
        return 1;
    }

    printf("Enter a string to concatenate: ");
    scanf("%s", input_str);

    // Fork the process
    p = fork();

    if (p < 0) {
        fprintf(stderr, "fork Failed" );
        return 1;
    }

    // Parent process
    else if (p > 0) {
        close(fd1[0]);  // Close reading end of first pipe
        close(fd2[0]);  // Close reading end of second pipe

        // Write input string to first pipe
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);  // Close writing end of first pipe

        // Wait for child to send a string back
        wait(NULL);

        // Read the concatenated string from the second pipe
        char final_str[100];
        read(fd2[0], final_str, sizeof(final_str));
        printf("Final output: %s\n", final_str);

        close(fd2[0]);  // Close reading end of second pipe
    }

    // Child process
    else {
        close(fd1[1]);  // Close writing end of first pipe
        close(fd2[0]);  // Close reading end of second pipe

        // Read a string from the first pipe
        char concat_str[100];
        read(fd1[0], concat_str, sizeof(concat_str));

        // Concatenate the fixed string
        strcat(concat_str, fixed_str);
        printf("Concatenated string: %s\n", concat_str);

        // Prompt for a second input
        char second_input[100];
        printf("Enter another string to concatenate: ");
        scanf("%s", second_input);

        // Concatenate the second input with the first
        strcat(concat_str, second_input);
        
        // Write the final concatenated string to the second pipe
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);  // Close writing end of second pipe

        close(fd1[0]);  // Close reading end of first pipe
        exit(0);
    }

    return 0;
}
