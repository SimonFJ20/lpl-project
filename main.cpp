#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
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

int main(int argc, char** argv)
{
    // auto text = read_file_to_string("../examples/test.lpl");
    if (argc < 2) {
        std::cerr << "fatal: lack of args :(\n"
                  << "USAGE: lpl <file>\n";
        exit(1);
    }
    auto text = read_file_to_string(argv[1]);
    std::cout << "Tokenizing\n";
    auto lexer = Lexer(text);
    auto tokens = lexer.tokenize();
    std::cout << "Lexer yeilded " << tokens.size() << " tokens\n";
    for (auto& t : tokens) {
        std::cout << "\t" << t.to_string() << "\n";
    }
    std::cout << "Parsing\n";
    auto parser = Parser(tokens);
    auto ast = parser.parse_expression();
    std::cout << ast->to_string() << "\n";
}
