# include <stdio.h>
# include <stdlib.h> // For atoi()


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

    FILE* fileptr;
    fileptr = fopen(FILEPATH, "r");
 
    if (fileptr == NULL) {
        perror("An error occurred.");
        return 1;
    }

    char line_buf[256];
    int line_count = 0;

    /*
    fgets(buffer, n, stream);
    - buffer = string buffer where input will be stored
    - n = maximum no. of chars to read (incl. null terminator)
    - stream = the input stream (e.g. stdin, fileptr)
    */
    while (line_count < lines && fgets(line_buf, sizeof(line_buf), fileptr) != NULL) {
        printf("%s", line_buf);
        line_count++;
    }

    fclose(fileptr);
    return 0;
}