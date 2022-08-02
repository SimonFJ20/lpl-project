#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <sstream>
#include <string>

std::string Parsed::BinaryOperation::to_string()
{
    const auto op = [&]() {
        switch (operator_) {
        case Parsed::BinaryOperator::Add: return "Add";
        case Parsed::BinaryOperator::Subtract: return "Subtract";
        case Parsed::BinaryOperator::Multiply: return "Multiply";
        case Parsed::BinaryOperator::Divide: return "Divide";
        case Parsed::BinaryOperator::Modulus: return "Modulus";
        case Parsed::BinaryOperator::Exponentiate: return "Exponentiate";
        case Parsed::BinaryOperator::LogicalAnd: return "LogicalAnd";
        case Parsed::BinaryOperator::LogicalOr: return "LogicalOr";
        case Parsed::BinaryOperator::BitwiseAnd: return "BitwiseAnd";
        case Parsed::BinaryOperator::BitwiseOr: return "BitwiseOr";
        case Parsed::BinaryOperator::BitwiseXor: return "BitwiseXor";
        case Parsed::BinaryOperator::BitwiseLeftShift:
            return "BitwiseLeftShift";
        case Parsed::BinaryOperator::BitwiseRightShift:
            return "BitwiseRightShift";
        case Parsed::BinaryOperator::LessThan: return "LessThan";
        case Parsed::BinaryOperator::LessThanEqual: return "LessThanEqual";
        case Parsed::BinaryOperator::GreaterThan: return "GreaterThan";
        case Parsed::BinaryOperator::GreaterThanEqual:
            return "GreaterThanEqual";
        case Parsed::BinaryOperator::Equal: return "Equal";
        case Parsed::BinaryOperator::NotEqual: return "NotEqual";
        }
        std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
                  << __LINE__ << ": in " << __func__ << "\n";
        exit(1);
    }();
    auto result = std::stringstream {};
    result << "BinaryOperation { left: " << left->to_string()
           << ", right: " << right->to_string() << ", operator: " << op << " }";
    return result.str();
}

std::string Parsed::UnaryOperation::to_string()
{
    const auto op = [&]() {
        switch (operator_) {
        case Parsed::UnaryOperator::LogicalNot: return "LogicalNot";
        case Parsed::UnaryOperator::BitwiseNot: return "BitwiseNot";
        case Parsed::UnaryOperator::Add: return "Add";
        case Parsed::UnaryOperator::Negate: return "Negate";
        }
        std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
                  << __LINE__ << ": in " << __func__ << "\n";
        exit(1);
    }();
    auto result = std::stringstream {};
    result << "UnaryOperation { expression: " << expression->to_string()
           << ", operator: " << op << " }";
    return result.str();
}

std::string Parsed::Int::to_string()
{
    auto result = std::stringstream {};
    result << "Int { " << value << " }";
    return result.str();
}

std::string Parsed::Float::to_string()
{
    auto result = std::stringstream {};
    result << "Float { " << value << " }";
    return result.str();
}

std::string Parsed::Char::to_string()
{
    auto result = std::stringstream {};
    result << "Char { '" << value << "' }";
    return result.str();
}

std::string Parsed::String::to_string()
{
    auto result = std::stringstream {};
    result << "String { \"" << value << "\" }";
    return result.str();
}

std::string Parsed::Bool::to_string()
{
    auto result = std::stringstream {};
    result << "Bool { " << (value ? "true" : "false") << " }";
    return result.str();
}

std::string token_type_to_string(TokenType type)
{
    switch (type) {
    case TokenType::EndOfFile: return "EndOfFile";
    case TokenType::Int: return "Int";
    case TokenType::Float: return "Float";
    case TokenType::Char: return "Char";
    case TokenType::String: return "String";
    case TokenType::Name: return "Name";
    case TokenType::If: return "If";
    case TokenType::While: return "While";
    case TokenType::Break: return "Break";
    case TokenType::Func: return "Func";
    case TokenType::Return: return "Return";
    case TokenType::Let: return "Let";
    case TokenType::False: return "False";
    case TokenType::True: return "True";
    case TokenType::Plus: return "Plus";
    case TokenType::Minus: return "Minus";
    case TokenType::Asterisk: return "Asterisk";
    case TokenType::Exponentation: return "Exponentation";
    case TokenType::Slash: return "Slash";
    case TokenType::Percent: return "Percent";
    case TokenType::LogicalNot: return "LogicalNot";
    case TokenType::LogicalAnd: return "LogicalAnd";
    case TokenType::LogicalOr: return "LogicalOr";
    case TokenType::BitwiseNot: return "BitwiseNot";
    case TokenType::BitwiseAnd: return "BitwiseAnd";
    case TokenType::BitwiseOr: return "BitwiseOr";
    case TokenType::BitwiseXor: return "BitwiseXor";
    case TokenType::BitwiseLeftShift: return "BitwiseLeftShift";
    case TokenType::BitwiseRightShift: return "BitwiseRightShift";
    case TokenType::LessThan: return "LessThan";
    case TokenType::LessThanEqual: return "LessThanEqual";
    case TokenType::GreaterThan: return "GreaterThan";
    case TokenType::GreaterThanEqual: return "GreaterThanEqual";
    case TokenType::Equal: return "Equal";
    case TokenType::NotEqual: return "NotEqual";
    case TokenType::AssignEqual: return "AssignEqual";
    case TokenType::LParen: return "LParen";
    case TokenType::RParen: return "RParen";
    case TokenType::LBrace: return "LBrace";
    case TokenType::RBrace: return "RBrace";
    case TokenType::LBracket: return "LBracket";
    case TokenType::RBracket: return "RBracket";
    case TokenType::Comma: return "Comma";
    case TokenType::Colon: return "Colon";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::ThinArrow: return "ThinArrow";
    }
    std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
              << __LINE__ << ": in " << __func__ << "\n";
    exit(1);
}

std::string Token::to_string()
{
    std::stringstream result {};
    result << "Token { type: " << token_type_to_string(type) << ", value: \""
           << value << "\"}";
    return result.str();
}
