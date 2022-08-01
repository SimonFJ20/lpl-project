#include "parser.h"
#include "lexer.h"
#include <iostream>
#include <memory>
#include <string>

// ParsedNode Parser::parse() {

// }

std::unique_ptr<Parsed::Expression> Parser::parse_expression()
{
    return parse_binary_operation();
}

std::unique_ptr<Parsed::Expression> Parser::parse_binary_operation() { }

std::unique_ptr<Parsed::Expression> Parser::parse_unary_operation()
{
    const auto& token = m_tokens[m_index];
    switch (token.type) {
    case TokenType::LogicalNot: step(); return std::make_unique<Parsed::>();
    }
}

std::unique_ptr<Parsed::Expression> Parser::parse_value()
{
    const auto& token = m_tokens[m_index];
    switch (token.type) {
    case TokenType::LParen: return parse_grouped_expression();
    case TokenType::Int: return parse_int();
    case TokenType::Float: return parse_float();
    case TokenType::Char: return parse_char();
    case TokenType::String: return parse_string();
    case TokenType::True: return parse_bool();
    case TokenType::False: return parse_bool();
    default:
        std::cout << "error occured " << __FILE__ << " " << __LINE__ << " "
                  << __FUNCTION__ << "\n";
        exit(1);
    }
}

std::unique_ptr<Parsed::Expression> Parser::parse_grouped_expression()
{
    step();
    auto expression = parse_expression();
    if (m_tokens[m_index].type != TokenType::RParen)
        error_and_exit("expected `)`");
    step();
    return expression;
}

std::unique_ptr<Parsed::Int> Parser::parse_int()
{
    const auto& token = m_tokens[m_index];
    step();
    return std::make_unique<Parsed::Int>(std::stoi(token.value));
}

std::unique_ptr<Parsed::Float> Parser::parse_float()
{
    const auto& token = m_tokens[m_index];
    step();
    return std::make_unique<Parsed::Float>(std::stof(token.value));
}

std::unique_ptr<Parsed::Char> Parser::parse_char()
{
    const auto& token = m_tokens[m_index];
    const auto value = [&]() {
        if (token.value[0] == '\\') {
            return unescape_char_value(token.value[1]);
        } else {
            return token.value[0];
        }
    }();
    step();
    return std::make_unique<Parsed::Char>(value);
}

std::unique_ptr<Parsed::String> Parser::parse_string()
{
    const auto& token = m_tokens[m_index];
    step();
    return std::make_unique<Parsed::String>(
        unescape_string_value(token.value.substr(1, token.value.length() - 2)));
}

std::string Parser::unescape_string_value(const std::string& value)
{
    auto result = std::string {};
    bool escaped = false;
    for (const auto c : value) {
        if (escaped) {
            escaped = false;
            result.push_back(unescape_char_value(c));
        } else {
            if (c == '\\')
                escaped = true;
            else
                result.push_back(c);
        }
    }
    return result;
}

char Parser::unescape_char_value(const char c)
{
    switch (c) {
    case 't': return '\t';
    case 'r': return '\r';
    case 'n': return '\n';
    case '0': return '\0';
    default: return c;
    }
}

std::unique_ptr<Parsed::Bool> Parser::parse_bool()
{
    const auto& token = m_tokens[m_index];
    const auto& value = [&]() {
        if (token.value.compare("false") == 0)
            return false;
        else if (token.value.compare("true") == 0)
            return true;
        else {
            std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
                      << __LINE__ << ": in " << __func__ << "\n";
            exit(1);
        }
    }();
    step();
    return std::make_unique<Parsed::Bool>(value);
}

bool Parser::done() { return m_index >= m_tokens.size(); }

void Parser::step() { m_index++; }

void Parser::error_and_exit(const std::string& msg)
{
    std::cerr << "ParserError: " << msg
              << "\n    // TODO handle errors in parser\n";
    exit(1);
}
