# include <string>
# include <iostream>
# include <fstream>
# include <cstdlib> // For atoi()


const std::string FILEPATH = "/usr/share/dict/words";

// using namespace std; --> Can include this line to avoid having to write "std::"

int main(int argc, char* argv[]) {
    // Check if file was run with corrent no. of arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <no. of lines to read>" << std::endl;
        return 1;
    }

    // Check if file was run with an integer argument
    int lines = atoi(argv[1]);
    if (lines <= 0) {
        std::cerr << "Error: Number of lines must be a natural number." << std::endl;
        return 1;
    }

    std::ifstream file_input_stream(FILEPATH);

    if (!file_input_stream.is_open()) {
        std::cerr << "An error occurred." << std::endl;
        return 1;
    }

    std::string line; // Placeholder to hold each line
    int line_count = 0;

    while (line_count < lines && std::getline(file_input_stream, line)) {
        std::cout << line << std::endl;
        line_count++;
    }

    return 0;
}