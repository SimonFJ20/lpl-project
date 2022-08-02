#pragma once

#include "lexer.h"
#include <memory>
#include <optional>
#include <vector>

namespace Parsed {

struct Node {
    virtual ~Node() {};
    virtual std::string to_string() = 0;
};

enum class ExpressionType {
    BinaryOperation,
    UnaryOperation,
    Int,
    Float,
    Char,
    String,
    Bool,
};

struct Expression : Node {
    virtual ~Expression() {};
    virtual ExpressionType expression_type() = 0;
};

enum class BinaryOperator {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulus,
    Exponentiate,
    LogicalAnd,
    LogicalOr,
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
};

struct BinaryOperation : Expression {
    BinaryOperation(std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right, BinaryOperator operator_)
        : left { std::move(left) }
        , right { std::move(right) }
        , operator_ { operator_ }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override
    {
        return ExpressionType::BinaryOperation;
    }

    std::unique_ptr<Expression> left, right;
    const BinaryOperator operator_;
};

enum class UnaryOperator {
    LogicalNot,
    BitwiseNot,
    Add,
    Negate,
};

struct UnaryOperation : Expression {
    UnaryOperation(
        std::unique_ptr<Expression> expression, UnaryOperator operator_)
        : expression { std::move(expression) }
        , operator_ { operator_ }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override
    {
        return ExpressionType::UnaryOperation;
    }

    std::unique_ptr<Expression> expression;
    const UnaryOperator operator_;
};

struct Int : Expression {
    Int(int value)
        : value { value }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override { return ExpressionType::Int; }

    const int value;
};

struct Float : Expression {
    Float(double value)
        : value { value }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override { return ExpressionType::Float; }

    const double value;
};

struct Char : Expression {
    Char(char value)
        : value { value }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override { return ExpressionType::Char; }

    const char value;
};

struct String : Expression {
    String(std::string value)
        : value { value }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override { return ExpressionType::String; }

    const std::string value;
};

struct Bool : Expression {
    Bool(bool value)
        : value { value }
    {
    }
    std::string to_string() override;
    ExpressionType expression_type() override { return ExpressionType::Bool; }

    const bool value;
};

}

class Parser {
public:
    Parser(const std::vector<Token>& tokens)
        : m_tokens { tokens }
    {
    }

    std::unique_ptr<Parsed::Node> parse();
    std::unique_ptr<Parsed::Expression> parse_expression();
    std::unique_ptr<Parsed::Expression> parse_binary_operation();
    int binary_operator_precedence(Parsed::BinaryOperator op);
    std::unique_ptr<Parsed::Expression> parse_unary_operation();
    std::optional<Parsed::BinaryOperator> maybe_parse_binary_operator();
    std::unique_ptr<Parsed::Expression> parse_value();
    std::unique_ptr<Parsed::Expression> parse_grouped_expression();
    std::unique_ptr<Parsed::Int> parse_int();
    std::unique_ptr<Parsed::Float> parse_float();
    std::unique_ptr<Parsed::Char> parse_char();
    std::unique_ptr<Parsed::String> parse_string();
    std::unique_ptr<Parsed::Bool> parse_bool();
    std::string unescape_string_value(const std::string& value);
    char unescape_char_value(char c);
    bool done();
    void step();
    void error_and_exit(const std::string& msg);

private:
    const std::vector<Token>& m_tokens;
    size_t m_index { 0 };
};
