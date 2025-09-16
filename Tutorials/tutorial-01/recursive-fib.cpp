#include <iostream> // used for std::cout and std::endl
#include <string> // used for std::stoi()

int fib(int n) {
    if (n <= 1) {
        return n;
    }

    return fib(n - 1) + fib(n - 2);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./recursive-fib <integer>" << std::endl;
        return 1;
    }

    int n = std::stoi(argv[1]);
    std::cout << fib(n) << std::endl;

    return 0;
}