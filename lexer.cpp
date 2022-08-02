#include "lexer.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    if (m_text.length() == 0) {
        tokens.push_back(Token(TokenType::EndOfFile, "", pos(0)));
        return tokens;
    }

    while (m_index < m_text.length()) {
        if (std::isdigit(m_text[m_index])) {
            tokens.push_back(make_number());
        } else if (std::isalpha(m_text[m_index]) || m_text[m_index] == '_') {
            tokens.push_back(make_name_or_keyword());
        } else if (std::isspace(m_text[m_index])) {
            step();
        } else {
            auto single_char = [&](TokenType type) {
                auto t = Token(type, m_text.substr(m_index, 1), pos(1));
                step();
                return t;
            };
            switch (m_text[m_index]) {
            case '\'': tokens.push_back(make_char()); break;
            case '\"': tokens.push_back(make_string()); break;
            case '+': tokens.push_back(single_char(TokenType::Plus)); break;
            case '-':
                tokens.push_back(make_single_or_double(
                    TokenType::Minus, TokenType::ThinArrow, '>'));
                break;
            case '*':
                tokens.push_back(make_single_or_double(
                    TokenType::Asterisk, TokenType::Exponentation, '*'));
                break;
            case '/': push_slash_or_comment(tokens); break;
            case '%': tokens.push_back(single_char(TokenType::Percent)); break;
            case '!':
                tokens.push_back(make_single_or_double(
                    TokenType::LogicalNot, TokenType::NotEqual, '='));
                break;
            case '&':
                tokens.push_back(make_single_or_double(
                    TokenType::BitwiseAnd, TokenType::LogicalAnd, '&'));
                break;
            case '|':
                tokens.push_back(make_single_or_double(
                    TokenType::BitwiseOr, TokenType::LogicalOr, '|'));
                break;
            case '~':
                tokens.push_back(single_char(TokenType::BitwiseNot));
                break;
            case '^':
                tokens.push_back(single_char(TokenType::BitwiseXor));
                break;
            case '<':
                tokens.push_back(make_single_or_two_double(TokenType::LessThan,
                    TokenType::LessThanEqual, '=', TokenType::BitwiseLeftShift,
                    '<'));
                break;
            case '>':
                tokens.push_back(make_single_or_two_double(
                    TokenType::GreaterThan, TokenType::GreaterThanEqual, '=',
                    TokenType::BitwiseRightShift, '>'));
                break;
            case '=':
                tokens.push_back(make_single_or_double(
                    TokenType::AssignEqual, TokenType::Equal, '='));
                break;
            case '(': tokens.push_back(single_char(TokenType::LParen)); break;
            case ')': tokens.push_back(single_char(TokenType::RParen)); break;
            case '{': tokens.push_back(single_char(TokenType::LBrace)); break;
            case '}': tokens.push_back(single_char(TokenType::RBrace)); break;
            case '[': tokens.push_back(single_char(TokenType::LBracket)); break;
            case ']': tokens.push_back(single_char(TokenType::RBracket)); break;
            case ',': tokens.push_back(single_char(TokenType::Comma)); break;
            case ':': tokens.push_back(single_char(TokenType::Colon)); break;
            case ';':
                tokens.push_back(single_char(TokenType::Semicolon));
                break;
            default:
                std::stringstream errormsg {};
                errormsg << "unexpected char '" << m_text[m_index] << "'";
                print_error(errormsg.str());
                exit(1);
            }
        }
    }
    tokens.push_back(Token(TokenType::EndOfFile, "", pos(0)));
    return tokens;
}

Token Lexer::make_number()
{
    std::string value { m_text[m_index] };
    step();
    int dots = 0;
    while (
        !done() && (std::isdigit(m_text[m_index]) || m_text[m_index] == '.')) {
        if (m_text[m_index] == '.' && dots < 1) {
            dots++;
        } else if (dots >= 1) {
            break;
        }
        value.push_back(m_text[m_index]);
        step();
    }
    const auto type = m_text.find('.') != std::string::npos ? TokenType::Float
                                                            : TokenType::Int;
    return Token(type, value, pos(value.length()));
}

Token Lexer::make_char()
{
    const auto check_done = [&]() {
        if (done())
            error_and_exit("unexpected end of char literal");
    };
    std::string value { m_text[m_index] };
    step();
    check_done();
    value.push_back(m_text[m_index]);
    if (m_text[m_index] == '\\') {
        step();
        check_done();
        value.push_back(m_text[m_index]);
    }
    step();
    check_done();
    if (m_text[m_index] != '\'')
        error_and_exit("expected `'` at end of char literal");
    value.push_back(m_text[m_index]);
    step();
    return Token(TokenType::Char, value, pos(value.length()));
}

Token Lexer::make_string()
{
    std::string value { m_text[m_index] };
    step();
    bool escaped = false;
    while (!done() && !(!escaped && m_text[m_index] == '\"')) {
        if (escaped)
            escaped = false;
        else if (m_text[m_index] == '\\')
            escaped = true;
        value.push_back(m_text[m_index]);
        step();
    }
    if (done() || m_text[m_index] != '\"')
        error_and_exit("expected `\"` at end of string literal");
    value.push_back(m_text[m_index]);
    step();
    return Token(TokenType::String, value, pos(value.length()));
}

Token Lexer::make_name_or_keyword()
{
    std::string value { m_text[m_index] };
    step();
    while (
        !done() && (std::isalpha(m_text[m_index]) || m_text[m_index] == '_')) {
        value.push_back(m_text[m_index]);
        step();
    }
    return Token(identifier_token_type(value), value, pos(value.length()));
}

TokenType Lexer::identifier_token_type(const std::string& value)
{
    if (value.compare("if") == 0)
        return TokenType::If;
    else if (value.compare("while") == 0)
        return TokenType::While;
    else if (value.compare("break") == 0)
        return TokenType::Break;
    else if (value.compare("func") == 0)
        return TokenType::Func;
    else if (value.compare("return") == 0)
        return TokenType::Return;
    else if (value.compare("let") == 0)
        return TokenType::Let;
    else if (value.compare("false") == 0)
        return TokenType::False;
    else if (value.compare("true") == 0)
        return TokenType::True;
    else
        return TokenType::Name;
}

Token Lexer::make_single_or_double(
    const TokenType case_single, const TokenType case_double, const char second)
{
    const std::string value { m_text[m_index] };
    step();
    if (!done() && m_text[m_index] == second) {
        const Token t(
            case_double, value + m_text[m_index], pos(value.length() + 1));
        step();
        return t;
    } else {
        return Token(case_single, value, pos(value.length()));
    }
}

Token Lexer::make_single_or_two_double(const TokenType case_single,
    const TokenType case_double_a, const char second_a,
    const TokenType case_double_b, const char second_b)
{
    const std::string value { m_text[m_index] };
    const auto single_or_double_a
        = make_single_or_double(case_single, case_double_a, second_a);
    if (single_or_double_a.type == case_single && !done()
        && m_text[m_index] == second_b) {
        const Token t(
            case_double_b, value + m_text[m_index], pos(value.length() + 1));
        step();
        return t;
    } else {
        return single_or_double_a;
    }
}

void Lexer::push_slash_or_comment(std::vector<Token>& tokens)
{
    const std::string value { m_text[m_index] };
    const auto p = pos(value.length());
    step();
    if (!done() && m_text[m_index] == '/') {
        while (!done() && m_text[m_index] != '\n')
            step();
    } else if (!done() && m_text[m_index] == '*') {
        step();
        if (done())
            error_and_exit("unexpected end of multi-line comment");
        auto last = m_text[m_index];
        step();
        while (!done() && !(last == '*' && m_text[m_index] == '/')) {
            last = m_text[m_index];
            step();
        }
        if (done() || last != '*' || m_text[m_index] != '/') {
            error_and_exit("unexpected end of multi-line comment");
        }
        step();
    } else {
        tokens.push_back(Token(TokenType::Slash, value, p));
    }
}

bool Lexer::done() { return m_index >= m_text.length(); }

void Lexer::step()
{
    m_index++;
    if (!done()) {
        if (m_text[m_index - 1] == '\n') {
            m_row++;
            m_col = 1;
        } else {
            m_col++;
        }
    }
}

void Lexer::print_error(const std::string& msg)
{
    const auto span = [&]() {
        const size_t start_of_line = m_index - (m_col - 1);
        size_t i = start_of_line;
        while (i < m_text.size() && m_text[i] != '\n')
            i++;
        return i - (m_index - m_col + 1);
    }();
    // std::cout << "m_index = " << m_index << ", "
    //           << "m_col = " << m_col << ", "
    //           << "m_row = " << m_row << ", "
    //           << "span = " << span << "\n";
    std::cerr << "LexerError: " << msg << "\n\n"
              << m_row << ":\t" << m_text.substr(m_index - (m_col - 1), span)
              << "\n\t" << std::string((m_col - 1), ' ') << "^ " << msg
              << "\n\n";
}

void Lexer::error_and_exit(const std::string& msg)
{
    print_error(msg);
    exit(1);
}

Position Lexer::pos(int length)
{
    return Position(m_index, length, m_col, m_row);
}
