#include "lexer.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    if (m_text.length() == 0) {
        tokens.push_back(Token(TokenType::EndOfFile, "\0", pos(0)));
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
            case '+': tokens.push_back(single_char(TokenType::Plus)); break;
            case '-':
                tokens.push_back(make_single_or_double(
                    TokenType::Minus, TokenType::ThinArrow, '>'));
                break;
            case '*': tokens.push_back(single_char(TokenType::Asterisk)); break;
            case '/': tokens.push_back(single_char(TokenType::Slash)); break;
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
                tokens.push_back(make_single_or_double(
                    TokenType::LessThan, TokenType::LessThanEqual, '='));
                break;
            case '>':
                tokens.push_back(make_single_or_double(
                    TokenType::GreaterThan, TokenType::GreaterThanEqual, '='));
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
    tokens.push_back(Token(TokenType::EndOfFile, "\0", pos(0)));
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
    auto type = m_text.find('.') != std::string::npos ? TokenType::Float
                                                      : TokenType::Int;
    return Token(type, value, pos(value.length()));
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
    if (value.compare("if"))
        return TokenType::If;
    else if (value.compare("while"))
        return TokenType::While;
    else if (value.compare("break"))
        return TokenType::Break;
    else if (value.compare("func"))
        return TokenType::Func;
    else if (value.compare("return"))
        return TokenType::Return;
    else if (value.compare("let"))
        return TokenType::Let;
    else if (value.compare("false"))
        return TokenType::False;
    else if (value.compare("true"))
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
    const auto span = [this]() {
        auto pos = m_text.find('\n', m_index + 1);
        if (pos == std::string::npos)
            return m_text.length() - m_index + 2;
        else
            return pos - (m_index - m_col + 1);
    }();
    // std::cout
    //     << "m_index = " << m_index << ", "
    //     << "m_col = " << m_col << ", "
    //     << "m_row = " << m_row << ", "
    //     << "span = " << span << "\n";
    std::cerr << "error: " << msg << "\n\n"
              << m_row << ":\t" << m_text.substr(m_index - (m_col - 1), span)
              << "\n\t" << std::string((m_col - 1), ' ') << "^ " << msg
              << "\n\n";
}

Position Lexer::pos(int length)
{
    return Position(m_index, length, m_col, m_row);
}

std::string token_type_to_string(TokenType type)
{
    switch (type) {
    case EndOfFile: return "EndOfFile";
    case Int: return "Int";
    case Float: return "Float";
    case Name: return "Name";
    case If: return "If";
    case While: return "While";
    case Break: return "Break";
    case Func: return "Func";
    case Return: return "Return";
    case Let: return "Let";
    case False: return "False";
    case True: return "True";
    case Plus: return "Plus";
    case Minus: return "Minus";
    case Asterisk: return "Asterisk";
    case Slash: return "Slash";
    case Percent: return "Percent";
    case LogicalNot: return "LogicalNot";
    case LogicalAnd: return "LogicalAnd";
    case LogicalOr: return "LogicalOr";
    case BitwiseNot: return "BitwiseNot";
    case BitwiseAnd: return "BitwiseAnd";
    case BitwiseOr: return "BitwiseOr";
    case BitwiseXor: return "BitwiseXor";
    case LessThan: return "LessThan";
    case LessThanEqual: return "LessThanEqual";
    case GreaterThan: return "GreaterThan";
    case GreaterThanEqual: return "GreaterThanEqual";
    case Equal: return "Equal";
    case NotEqual: return "NotEqual";
    case AssignEqual: return "AssignEqual";
    case LParen: return "LParen";
    case RParen: return "RParen";
    case LBrace: return "LBrace";
    case RBrace: return "RBrace";
    case LBracket: return "LBracket";
    case RBracket: return "RBracket";
    case Comma: return "Comma";
    case Colon: return "Colon";
    case Semicolon: return "Semicolon";
    case ThinArrow: return "ThinArrow";
    }
    std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
              << __LINE__ << ": in " << __func__ << "\n";
    exit(1);
}

std::string Token::to_string()
{
    std::string result {};
    result += "(";
    result += token_type_to_string(type);
    result += ", \"";
    result += value;
    result += "\")";
    return result;
}
