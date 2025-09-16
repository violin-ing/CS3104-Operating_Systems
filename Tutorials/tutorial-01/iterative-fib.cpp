#include <iostream> // used for std::cout and std::endl
#include <string> // used for std::stoi()

int fib(int n) {
    int a = 0;
    int b = 1;
    for (int i = 0; i < n; i++) {
        int tmp = a;
        a = b;
        b += tmp;
    }

    return a;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./iterative-fib <integer>" << std::endl;
        return 1;
    }

    int n = std::stoi(argv[1]);
    std::cout << fib(n) << std::endl;

    return 0;
}