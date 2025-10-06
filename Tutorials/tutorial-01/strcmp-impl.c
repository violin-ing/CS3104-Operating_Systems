#include <stdio.h>
#include <stdlib.h> // for abs()

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++; // str1 += sizeof(char)
        str2++;
    }

    return (unsigned char) *str1 - (unsigned char) *str2;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./strcmp-impl <s1> <s2>\n");
        return 1;
    }

    int res = strcmp(argv[1], argv[2]);
    printf("%d\n", res == 0 ? 0 : res / abs(res));

    return 0;
}
