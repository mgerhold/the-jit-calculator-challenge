#include <cassert>
#include <functional>
#include <jit_calculator/parser.hpp>
#include <jit_calculator/precedence.hpp>
#include <jit_calculator/token.hpp>
#include <lib2k/types.hpp>
#include <span>
#include <sstream>
#include <vector>
#include "utils.hpp"

class Parser final {
private:
    std::span<Token const> m_tokens;
    usize m_index = 0;

public:
    [[nodiscard]] explicit Parser(std::span<Token const> const tokens)
        : m_tokens{ tokens } {}

    [[nodiscard]] std::unique_ptr<Expression> expression() {
        return expression(Precedence::Unknown);
    }

    [[nodiscard]] bool is_at_end() const {
        return m_index >= m_tokens.size() or m_tokens[m_index].type == EndOfFile;
    }

private:
    [[nodiscard]] Token const& current() const;
    void advance();
    void consume(TokenType type, std::string const& error_message);
    [[nodiscard]] std::unique_ptr<Expression> expression(Precedence precedence);
    [[nodiscard]] std::unique_ptr<Expression> number();
    [[nodiscard]] std::unique_ptr<Expression> unary();
    [[nodiscard]] std::unique_ptr<Expression> binary(std::unique_ptr<Expression> left_operand);
    [[nodiscard]] std::unique_ptr<Expression> grouping();
    [[nodiscard]] std::unique_ptr<Expression> identifier();
    [[nodiscard]] ParserTableRecord const& current_table_record() const;

    // clang-format off
    using enum TokenType;
    static inline auto parser_table = create_parser_table(
        ParserTableEntry<Integer>           { &Parser::number,     nullptr,         Precedence::Unknown },
        ParserTableEntry<Plus>              { &Parser::unary,      &Parser::binary, Precedence::Sum     },
        ParserTableEntry<Minus>             { &Parser::unary,      &Parser::binary, Precedence::Sum     },
        ParserTableEntry<Asterisk>          { nullptr,             &Parser::binary, Precedence::Product },
        ParserTableEntry<Slash>             { nullptr,             &Parser::binary, Precedence::Product },
        ParserTableEntry<LeftParenthesis>   { &Parser::grouping,   nullptr,         Precedence::Unknown },
        ParserTableEntry<RightParenthesis>  { nullptr,             nullptr,         Precedence::Unknown },
        ParserTableEntry<Identifier>        { &Parser::identifier, nullptr,         Precedence::Unknown },
        ParserTableEntry<EndOfFile>         { nullptr,             nullptr,         Precedence::Unknown }
    );
    // clang-format on
};

[[nodiscard]] Token const& Parser::current() const {
    if (is_at_end()) {
        return m_tokens.back();
    }
    return m_tokens[m_index];
}

void Parser::advance() {
    if (not is_at_end()) {
        ++m_index;
    }
}

void Parser::consume(TokenType const type, std::string const& error_message) {
    if (current().type == type) {
        advance();
    } else {
        throw std::runtime_error{ error_message };
    }
}

[[nodiscard]] std::unique_ptr<Expression> Parser::expression(Precedence const precedence) {
    auto const prefix_parser = current_table_record().prefix_parser;
    if (prefix_parser == nullptr) {
        throw std::runtime_error{ "Unexpected token." };
    }
    auto first_operand = std::invoke(prefix_parser, *this);

    // This could still only be the first operand of a binary operator.
    while (true) {
        auto const [_, infix_parser, infix_precedence] = current_table_record();
        if (infix_precedence <= precedence or infix_parser == nullptr) {
            return first_operand;
        }
        first_operand = std::invoke(infix_parser, *this, std::move(first_operand));
    }
}

[[nodiscard]] std::unique_ptr<Expression> Parser::number() {
    auto stream = std::istringstream{ std::string{ current().lexeme } };
    auto value = i64{};
    stream >> value;
    assert(stream and stream.eof());
    advance();
    return std::make_unique<::Number>(value);
}

[[nodiscard]] std::unique_ptr<Expression> Parser::unary() {
    auto const type = current().type;
    advance();
    return std::make_unique<UnaryOperator>(type, expression(Precedence::Unary));
}

[[nodiscard]] std::unique_ptr<Expression> Parser::binary(std::unique_ptr<Expression> left_operand) {
    auto const type = current().type;
    advance();
    auto const precedence = parser_table.at(std::to_underlying(type)).infix_precedence;
    auto right_operand = expression(precedence);
    return std::make_unique<BinaryOperator>(std::move(left_operand), type, std::move(right_operand));
}

[[nodiscard]] std::unique_ptr<Expression> Parser::grouping() {
    advance();  // Consume '('.
    auto inner_expression = expression();
    consume(RightParenthesis, "Expected ')'.");
    return inner_expression;
}

[[nodiscard]] std::unique_ptr<Expression> Parser::identifier() {
    assert(current().lexeme.size() == 1);
    auto const name = current().lexeme.front();
    advance();
    return std::make_unique<::Identifier>(name);
}

[[nodiscard]] ParserTableRecord const& Parser::current_table_record() const {
    return parser_table.at(std::to_underlying(current().type));
}

[[nodiscard]] std::unique_ptr<Expression> parse(std::span<Token const> tokens) {
    auto parser = Parser{ tokens };
    auto result = parser.expression();
    if (not parser.is_at_end()) {
        throw std::runtime_error{ "Excess tokens." };
    }
    return result;
}
