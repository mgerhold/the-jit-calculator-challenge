#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "instruction.hpp"
#include "token.hpp"

class Expression {
public:
    Expression() = default;
    Expression(Expression const& other) = default;
    Expression(Expression&& other) noexcept = default;
    Expression& operator=(Expression const& other) = default;
    Expression& operator=(Expression&& other) noexcept = default;
    virtual ~Expression() = default;

    virtual void emit(std::vector<Instruction>& instructions) const = 0;

    virtual void print_to_stream(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, Expression const& expression) {
        expression.print_to_stream(os);
        return os;
    }
};

class Number final : public Expression {
private:
    i64 m_value;

public:
    explicit Number(i64 value);

    void emit(std::vector<Instruction>& instructions) const override;
    void print_to_stream(std::ostream& os) const override;
};

class UnaryOperator final : public Expression {
private:
    TokenType m_type;
    std::unique_ptr<Expression> m_operand;

public:
    explicit UnaryOperator(TokenType type, std::unique_ptr<Expression> operand);
    void emit(std::vector<Instruction>& instructions) const override;
    void print_to_stream(std::ostream& os) const override;
};

class BinaryOperator final : public Expression {
private:
    std::unique_ptr<Expression> m_left_operand;
    TokenType m_type;
    std::unique_ptr<Expression> m_right_operand;

public:
    explicit BinaryOperator(
        std::unique_ptr<Expression> left_operand,
        TokenType type,
        std::unique_ptr<Expression> right_operand
    );

    void print_to_stream(std::ostream& os) const override;
    void emit(std::vector<Instruction>& instructions) const override;
};

class Identifier final : public Expression {
private:
    char m_name;

public:
    explicit Identifier(char name);
    void emit(std::vector<Instruction>& instructions) const override;
    void print_to_stream(std::ostream& os) const override;
};
