#include "parser.h"
#include "lexer.h"
#include <iostream>
#include <memory>
#include <optional>
#include <string>

// ParsedNode Parser::parse() {

// }

std::optional<std::unique_ptr<Parsed::Assignment>>
Parser::maybe_parse_assignment()
{
    const auto original_index = m_index;
    auto target = parse_expression();
    if (current().type == TokenType::AssignEqual) {
        step();
        auto value = parse_expression();
        return std::make_unique<Parsed::Assignment>(
            std::move(target), std::move(value));
    } else {
        m_index = original_index;
        return std::nullopt;
    }
}

std::unique_ptr<Parsed::Expression> Parser::parse_expression()
{
    switch (current().type) {
    case TokenType::LBrace: return parse_block();
    default: return parse_binary_operation();
    }
}

std::unique_ptr<Parsed::Block> Parser::parse_block()
{
    step();
    auto statements = std::vector<std::unique_ptr<Parsed::Statement>> {};
    auto value
        = std::optional<std::unique_ptr<Parsed::Expression>> { std::nullopt };
    while (!done() && current().type != TokenType::RBrace) {
        if (auto statement = maybe_parse_assignment()) {
            statements.push_back(std::move(*statement));
            if (current().type != TokenType::Semicolon)
                error_and_exit("expected `;`");
            step();
        } else {
            auto expression = parse_expression();
            if (current().type == TokenType::Semicolon) {
                statements.push_back(
                    std::make_unique<Parsed::ExpressionStatement>(
                        std::move(expression)));
                step();
            } else if (current().type == TokenType::RBrace) {
                value = std::move(expression);
            } else {
                error_and_exit("expected `;` or `}`");
            }
        }
    }
    if (done() || current().type != TokenType::RBrace)
        error_and_exit("expected `}`");
    step();
    return std::make_unique<Parsed::Block>(
        std::move(statements), std::move(value));
}

std::unique_ptr<Parsed::Expression> Parser::parse_binary_operation()
{
    auto expression_stack = std::vector<std::unique_ptr<Parsed::Expression>> {};
    auto operator_stack = std::vector<Parsed::BinaryOperator> {};
    const auto pop_expression = [&]() {
        auto expression = std::move(expression_stack.back());
        expression_stack.pop_back();
        return expression;
    };
    const auto pop_operator = [&]() {
        auto operator_ = operator_stack.back();
        operator_stack.pop_back();
        return operator_;
    };
    expression_stack.push_back(parse_unary_operation());
    auto last_precedence = 20;
    while (!done()) {
        const auto operator_ = maybe_parse_binary_operator();
        if (!operator_.has_value())
            break;
        const auto precedence = binary_operator_precedence(operator_.value());
        auto right = parse_unary_operation();
        while (precedence <= last_precedence && expression_stack.size() > 1) {
            auto right = pop_expression();
            const auto operator_ = pop_operator();
            last_precedence = binary_operator_precedence(operator_);
            if (last_precedence < precedence) {
                expression_stack.push_back(std::move(right));
                operator_stack.push_back(operator_);
                break;
            }
            auto left = pop_expression();
            expression_stack.push_back(
                std::make_unique<Parsed::BinaryOperation>(
                    std::move(left), std::move(right), operator_));
        }
        expression_stack.push_back(std::move(right));
        operator_stack.push_back(operator_.value());
    }
    while (expression_stack.size() > 1) {
        auto right = pop_expression();
        auto left = pop_expression();
        expression_stack.push_back(std::make_unique<Parsed::BinaryOperation>(
            std::move(left), std::move(right), pop_operator()));
    }
    return std::move(expression_stack[0]);
}

std::unique_ptr<Parsed::Expression> Parser::parse_unary_operation()
{
    const auto& token = current();
    const auto step_and_make_operation = [&](Parsed::UnaryOperator operator_) {
        step();
        return std::make_unique<Parsed::UnaryOperation>(
            parse_expression(), operator_);
    };
    switch (token.type) {
    case TokenType::LogicalNot:
        return step_and_make_operation(Parsed::UnaryOperator::LogicalNot);
    case TokenType::BitwiseNot:
        return step_and_make_operation(Parsed::UnaryOperator::BitwiseNot);
    case TokenType::Plus:
        return step_and_make_operation(Parsed::UnaryOperator::Add);
    case TokenType::Minus:
        return step_and_make_operation(Parsed::UnaryOperator::Negate);
    default: return parse_call();
    }
}

std::unique_ptr<Parsed::Expression> Parser::parse_call()
{
    auto callee = parse_value();
    if (current().type == TokenType::LParen) {
        step();
        auto args = std::vector<std::unique_ptr<Parsed::Expression>> {};
        while (!done() && current().type != TokenType::RParen) {
            args.push_back(parse_expression());
            if (current().type == TokenType::RParen)
                break;
            else if (current().type != TokenType::Comma)
                error_and_exit("expected `,` or `)`");
            step();
        }
        if (current().type != TokenType::RParen)
            error_and_exit("expected `)`");
        step();
        return std::make_unique<Parsed::Call>(
            std::move(callee), std::move(args));
    } else {
        return callee;
    }
}

std::unique_ptr<Parsed::Expression> Parser::parse_value()
{
    switch (current().type) {
    case TokenType::LParen: return parse_grouped_expression();
    case TokenType::Int: return parse_int();
    case TokenType::Float: return parse_float();
    case TokenType::Char: return parse_char();
    case TokenType::String: return parse_string();
    case TokenType::True: return parse_bool();
    case TokenType::False: return parse_bool();
    case TokenType::Name: return parse_symbol();
    default:
        std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
                  << __LINE__ << ": in " << __func__ << "\n";
        exit(1);
    }
}

std::unique_ptr<Parsed::Expression> Parser::parse_grouped_expression()
{
    step();
    auto expression = parse_expression();
    if (current().type != TokenType::RParen)
        error_and_exit("expected `)`");
    step();
    return expression;
}

std::unique_ptr<Parsed::Int> Parser::parse_int()
{
    const auto& token = current();
    step();
    return std::make_unique<Parsed::Int>(std::stoi(token.value));
}

std::unique_ptr<Parsed::Float> Parser::parse_float()
{
    const auto& token = current();
    step();
    return std::make_unique<Parsed::Float>(std::stof(token.value));
}

std::unique_ptr<Parsed::Char> Parser::parse_char()
{
    const auto& token = current();
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
    const auto& token = current();
    step();
    return std::make_unique<Parsed::String>(
        unescape_string_value(token.value.substr(1, token.value.length() - 2)));
}

std::unique_ptr<Parsed::Bool> Parser::parse_bool()
{
    const auto& token = current();
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

std::unique_ptr<Parsed::Symbol> Parser::parse_symbol()
{
    const auto& token = current();
    step();
    return std::make_unique<Parsed::Symbol>(token.value);
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

constexpr char Parser::unescape_char_value(const char c) const
{
    switch (c) {
    case 't': return '\t';
    case 'r': return '\r';
    case 'n': return '\n';
    case '0': return '\0';
    default: return c;
    }
}
const Token& Parser::current() const { return m_tokens[m_index]; }

bool Parser::done() const { return m_index >= m_tokens.size(); }

void Parser::step() { m_index++; }

void Parser::error_and_exit(const std::string& msg)
{
    std::cerr << "ParserError: " << msg
              << "\n    // TODO handle errors in parser\n";
    exit(1);
}

constexpr int Parser::binary_operator_precedence(
    Parsed::BinaryOperator op) const
{
    switch (op) {
    case Parsed::BinaryOperator::Add: return 11;
    case Parsed::BinaryOperator::Subtract: return 11;
    case Parsed::BinaryOperator::Multiply: return 12;
    case Parsed::BinaryOperator::Divide: return 12;
    case Parsed::BinaryOperator::Modulus: return 12;
    case Parsed::BinaryOperator::Exponentiate: return 13;
    case Parsed::BinaryOperator::LogicalAnd: return 4;
    case Parsed::BinaryOperator::LogicalOr: return 3;
    case Parsed::BinaryOperator::BitwiseAnd: return 7;
    case Parsed::BinaryOperator::BitwiseOr: return 5;
    case Parsed::BinaryOperator::BitwiseXor: return 6;
    case Parsed::BinaryOperator::BitwiseLeftShift: return 10;
    case Parsed::BinaryOperator::BitwiseRightShift: return 10;
    case Parsed::BinaryOperator::LessThan: return 9;
    case Parsed::BinaryOperator::LessThanEqual: return 9;
    case Parsed::BinaryOperator::GreaterThan: return 9;
    case Parsed::BinaryOperator::GreaterThanEqual: return 9;
    case Parsed::BinaryOperator::Equal: return 8;
    case Parsed::BinaryOperator::NotEqual: return 8;
    }
    std::cerr << "internal: unexhaustive match at " << __FILE__ << ":"
              << __LINE__ << ": in " << __func__ << "\n";
    exit(1);
}

std::optional<Parsed::BinaryOperator> Parser::maybe_parse_binary_operator()
{
    const auto step_and = [&](Parsed::BinaryOperator op) {
        step();
        return op;
    };
    switch (current().type) {
    case TokenType::Plus: return step_and(Parsed::BinaryOperator::Add);
    case TokenType::Minus: return step_and(Parsed::BinaryOperator::Subtract);
    case TokenType::Asterisk: return step_and(Parsed::BinaryOperator::Multiply);
    case TokenType::Slash: return step_and(Parsed::BinaryOperator::Divide);
    case TokenType::Percent: return step_and(Parsed::BinaryOperator::Modulus);
    case TokenType::Exponentation:
        return step_and(Parsed::BinaryOperator::Exponentiate);
    case TokenType::LogicalAnd:
        return step_and(Parsed::BinaryOperator::LogicalAnd);
    case TokenType::LogicalOr:
        return step_and(Parsed::BinaryOperator::LogicalOr);
    case TokenType::BitwiseAnd:
        return step_and(Parsed::BinaryOperator::BitwiseAnd);
    case TokenType::BitwiseOr:
        return step_and(Parsed::BinaryOperator::BitwiseOr);
    case TokenType::BitwiseXor:
        return step_and(Parsed::BinaryOperator::BitwiseXor);
    case TokenType::BitwiseLeftShift:
        return step_and(Parsed::BinaryOperator::BitwiseLeftShift);
    case TokenType::BitwiseRightShift:
        return step_and(Parsed::BinaryOperator::BitwiseRightShift);
    case TokenType::LessThan: return step_and(Parsed::BinaryOperator::LessThan);
    case TokenType::LessThanEqual:
        return step_and(Parsed::BinaryOperator::LessThanEqual);
    case TokenType::GreaterThan:
        return step_and(Parsed::BinaryOperator::GreaterThan);
    case TokenType::GreaterThanEqual:
        return step_and(Parsed::BinaryOperator::GreaterThanEqual);
    case TokenType::Equal: return step_and(Parsed::BinaryOperator::Equal);
    case TokenType::NotEqual: return step_and(Parsed::BinaryOperator::NotEqual);
    default: return {};
    }
}
