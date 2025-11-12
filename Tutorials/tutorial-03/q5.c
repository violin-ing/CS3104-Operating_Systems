# include <stdio.h>
# include <stdlib.h> // For atoi()
# include <unistd.h>
# include <fcntl.h>
# include <sys/syscall.h>


char* FILEPATH = "/usr/share/dict/words";

int main(int argc, char* argv[]) {
    // Check if file was run with corrent no. of arguments
    if (argc != 2) {
        printf("Usage: %s <no. of lines to read>\n", argv[0]);
        return 1;
    }

    // Check if file was run with an integer argument
    int lines = atoi(argv[1]);
    if (lines <= 0) {
        printf("Error: Number of lines must be a natural number.\n");
        return 1;
    }

    /*
    syscall(operation_id, ...)
    - operation_id = the corresponding ID of the kernel operation to perform
    - ... = variadic arguments
    */

    int fd = syscall(__NR_openat, AT_FDCWD, FILEPATH, O_RDONLY, 0);
    if (fd == -1) {
        perror("An error occurred.");
        return 1;
    }

    char c;
    int line_c = 0;
    long bytes_read;

    /*
    syscall(__NR_read, fd, &buf, n); -> returns a long value indicating no. of bytes read
    - fd = file descriptor
    - &buf = address of buffer
    - n = maximum no. of bytes to read
    */
    while (line_c < lines && (bytes_read = syscall(__NR_read, fd, &c, 1)) == 1) {
        printf("%c", c);
        if (c == '\n') {
            line_c++;
        }
    }

    if (bytes_read == -1) {
        perror("An error occurred.");
        return 1;
    }

    syscall(__NR_close, fd);
    return 0;
}