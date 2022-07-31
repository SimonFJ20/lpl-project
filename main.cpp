#include "lexer.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

std::string read_file_to_string(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "error: file \"" << filename << "\" could not be opened\n";
        exit(1);
    }
    std::ostringstream ss {};
    ss << file.rdbuf();
    return ss.str();
}

int main()
{
    auto text = read_file_to_string("../examples/test.lpl");
    std::cout << "Compiling\n";
    Lexer lexer(text);
    auto tokens = lexer.tokenize();
    std::cout << "Lexer yeilded " << tokens.size() << " tokens\n";
    for (auto& t : tokens) {
        std::cout << "\t" << t.to_string() << "\n";
    }
}
