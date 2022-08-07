#pragma once

#include "lexer.h"
#include <memory>
#include <optional>
#include <vector>

namespace Parsed {

struct Node {
    virtual ~Node() = default;
    virtual std::string to_string() const = 0;
};

enum class StatementType {
    Assignment,
    Expression,
};

enum class ExpressionType {
    Block,
    BinaryOperation,
    UnaryOperation,
    Call,
    Int,
    Float,
    Char,
    String,
    Bool,
    Symbol,
};

struct Expression : public Node {
    virtual ~Expression() = default;
    constexpr virtual ExpressionType expression_type() const = 0;
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

struct BinaryOperation final : public Expression {
    BinaryOperation(std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right, BinaryOperator operator_)
        : left { std::move(left) }
        , right { std::move(right) }
        , operator_ { operator_ }
    {
    }
    ~BinaryOperation() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
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

struct UnaryOperation final : public Expression {
    UnaryOperation(
        std::unique_ptr<Expression> expression, UnaryOperator operator_)
        : expression { std::move(expression) }
        , operator_ { operator_ }
    {
    }
    ~UnaryOperation() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::UnaryOperation;
    }

    std::unique_ptr<Expression> expression;
    const UnaryOperator operator_;
};

struct Call final : public Expression {
    Call(std::unique_ptr<Expression> callee,
        std::vector<std::unique_ptr<Expression>> args)
        : callee { std::move(callee) }
        , args { std::move(args) }
    {
    }
    ~Call() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Call;
    }

    std::unique_ptr<Expression> callee;
    const std::vector<std::unique_ptr<Expression>> args;
};

struct Int final : public Expression {
    Int(int value)
        : value { value }
    {
    }
    ~Int() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Int;
    }

    const int value;
};

struct Float final : public Expression {
    Float(double value)
        : value { value }
    {
    }
    ~Float() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Float;
    }

    const double value;
};

struct Char final : public Expression {
    Char(char value)
        : value { value }
    {
    }
    ~Char() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Char;
    }

    const char value;
};

struct String final : public Expression {
    String(std::string value)
        : value { value }
    {
    }
    ~String() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::String;
    }

    const std::string value;
};

struct Bool final : public Expression {
    Bool(bool value)
        : value { value }
    {
    }
    ~Bool() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Bool;
    }

    const bool value;
};

struct Symbol final : public Expression {
    Symbol(const std::string value)
        : value { value }
    {
    }
    ~Symbol() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Symbol;
    }

    const std::string value;
};

struct Statement : public Node {
    virtual ~Statement() = default;
    constexpr virtual StatementType statement_type() const = 0;
};

struct Assignment : public Statement {
    Assignment(
        std::unique_ptr<Expression> target, std::unique_ptr<Expression> value)
        : target { std::move(target) }
        , value { std::move(value) }
    {
    }
    ~Assignment() = default;
    std::string to_string() const override;
    StatementType statement_type() const override
    {
        return StatementType::Assignment;
    }

    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;
};

struct ExpressionStatement : public Statement {
    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression { std::move(expression) }
    {
    }
    ~ExpressionStatement() = default;
    std::string to_string() const override;
    StatementType statement_type() const override
    {
        return StatementType::Expression;
    }

    std::unique_ptr<Expression> expression;
};

struct Block final : public Expression {
    Block(std::vector<std::unique_ptr<Statement>> statements,
        std::optional<std::unique_ptr<Expression>> value)
        : statements { std::move(statements) }
        , value { std::move(value) }
    {
    }
    ~Block() = default;
    std::string to_string() const override;
    constexpr ExpressionType expression_type() const override
    {
        return ExpressionType::Block;
    }

    std::vector<std::unique_ptr<Statement>> statements;
    std::optional<std::unique_ptr<Expression>> value;
};

}

class Parser {
public:
    Parser(const std::vector<Token>& tokens)
        : m_tokens { tokens }
    {
    }

    std::unique_ptr<Parsed::Node> parse();
    std::optional<std::unique_ptr<Parsed::Assignment>> maybe_parse_assignment();
    std::unique_ptr<Parsed::Expression> parse_expression();
    std::unique_ptr<Parsed::Block> parse_block();
    std::unique_ptr<Parsed::Expression> parse_binary_operation();
    constexpr int binary_operator_precedence(Parsed::BinaryOperator op) const;
    std::unique_ptr<Parsed::Expression> parse_unary_operation();
    std::optional<Parsed::BinaryOperator> maybe_parse_binary_operator();
    std::unique_ptr<Parsed::Expression> parse_call();
    std::unique_ptr<Parsed::Expression> parse_value();
    std::unique_ptr<Parsed::Expression> parse_grouped_expression();
    std::unique_ptr<Parsed::Int> parse_int();
    std::unique_ptr<Parsed::Float> parse_float();
    std::unique_ptr<Parsed::Char> parse_char();
    std::unique_ptr<Parsed::String> parse_string();
    std::unique_ptr<Parsed::Bool> parse_bool();
    std::unique_ptr<Parsed::Symbol> parse_symbol();
    std::string unescape_string_value(const std::string& value);
    constexpr char unescape_char_value(const char c) const;
    const Token& current() const;
    bool done() const;
    void step();
    void error_and_exit(const std::string& msg);

private:
    const std::vector<Token>& m_tokens;
    size_t m_index { 0 };
};
