# include <stdio.h>
# include <stdlib.h> // For atoi()
# include <unistd.h>
# include <fcntl.h> // For O_RDONLY constant
# include <sys/mman.h> // For mmap() & munmap()
# include <sys/stat.h> // For fstat()


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
        return 1;
    }

    struct stat s;
    if (fstat(fd, &s) == -1) {
        perror("An error occurred.");
        close(fd);
        return 1;
    }

    off_t size = s.st_size;

    char* buf = (char*) mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
        perror("An error occurred.");
        close(fd);
        return 1;
    }

    close(fd); // mmap holds a reference to the file by this point

    int line_c = 0;
    int poo = 0; // Used to navigate through buf
    char c; // Placeholder for chars from buf

    while (line_c < lines) {
        if (poo < size) {
            c = buf[poo];
            putchar(c);
            poo++;
            if (c == '\n') {
                line_c++;
            }
        } else {
            break;
        }
    }

    if (munmap(buf, size) == -1) {
        perror("An error occurred");
        return 1;
    }

    return 0;
}