#pragma once

#include <string>
#include <vector>

struct Position {
public:
    Position(size_t index, size_t length, size_t row, size_t col)
        : index(index)
        , length(length)
        , row(row)
        , col(col) {};

    const size_t index, length, row, col;
};

enum class TokenType {
    EndOfFile,
    Int,
    Float,
    Char,
    String,
    Name,
    If,
    While,
    Break,
    Func,
    Return,
    Let,
    False,
    True,
    Plus,
    Minus,
    Asterisk,
    Exponentation,
    Slash,
    Percent,
    LogicalNot,
    LogicalAnd,
    LogicalOr,
    BitwiseNot,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseLeftShift,
    BitwiseRightShift,
    LessThan,
    LessThanEqual,
    GreaterThan,
    GreaterThanEqual,
    Equal,
    NotEqual,
    AssignEqual,
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Colon,
    Semicolon,
    ThinArrow,
};

std::string token_type_to_string(TokenType type);

struct Token {
public:
    Token(TokenType type, std::string m_value, Position pos)
        : type { type }
        , value { m_value }
        , pos { pos }
    {
    }

    std::string to_string();

    const TokenType type;
    const std::string value;
    const Position pos;
};

class Lexer {
public:
    Lexer(std::string& text)
        : m_text { text }
    {
    }

    std::vector<Token> tokenize();

private:
    Token make_number();
    Token make_char();
    Token make_string();
    Token make_name_or_keyword();
    TokenType identifier_token_type(const std::string& value);
    Token make_single_or_double(const TokenType case_single,
        const TokenType case_double, const char second);
    Token make_single_or_two_double(const TokenType case_single,
        const TokenType case_double_a, const char second_a,
        const TokenType case_double_b, const char second_b);
    void push_slash_or_comment(std::vector<Token>& tokens);
    bool done();
    void step();
    void print_error(const std::string& msg);
    void error_and_exit(const std::string& msg);
    Position pos(int length);

    const std::string& m_text;
    size_t m_index { 0 };
    int m_row { 1 }, m_col { 1 };
};
