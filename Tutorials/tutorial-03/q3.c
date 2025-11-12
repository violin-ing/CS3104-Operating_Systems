# include <stdio.h>
# include <stdlib.h> // For atoi()
# include <unistd.h>
# include <fcntl.h> // For O_RDONLY constant


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

    int fd = open(FILEPATH, O_RDONLY);
    if (fd == -1) {
        perror("An error occurred.");
    }

    int line_count = 0;
    char c; // Placeholder 1-byte buffer
    ssize_t bytes_read; // Type used to store return value of read()

    /*
    read(fd, buffer, n);
    - fd = file descriptor
    - buffer = placeholder buffer to store the shit read
    - n = no. of bytes to read
    */

    // read() returns the no. of bytes read, and -1 if an error occurs
    while (line_count < lines && (bytes_read = read(fd, &c, 1)) == 1) {
        printf("%c", c);
        if (c == '\n') {
            line_count++;
        }
    }

    if (bytes_read == -1) {
        perror("An error occurred.");
    }

    close(fd);
    return 0;
}