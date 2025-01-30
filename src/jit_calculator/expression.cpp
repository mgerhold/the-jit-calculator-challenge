#include <jit_calculator/expression.hpp>

Number::Number(i64 const value)
    : m_value{ value } {}

void Number::emit(std::vector<Instruction>& instructions) const {
    instructions.emplace_back(ImmediateValue{ m_value });
}

void Number::print_to_stream(std::ostream& os) const {
    os << m_value;
}

UnaryOperator::UnaryOperator(TokenType const type, std::unique_ptr<Expression> operand)
    : m_type{ type }, m_operand{ std::move(operand) } {}

void UnaryOperator::emit(std::vector<Instruction>&) const {}

void UnaryOperator::print_to_stream(std::ostream& os) const {
    auto const operator_char = [&] {
        // clang-format off
        switch (m_type) {
            using enum TokenType;
            case Plus:  return '+';
            case Minus: return '-';
            default:    return '?';
        }
        // clang-format on
    }();
    os << operator_char << "(" << *m_operand << ')';
}

BinaryOperator::BinaryOperator(
    std::unique_ptr<Expression> left_operand,
    TokenType const type,
    std::unique_ptr<Expression> right_operand
)
    : m_left_operand{ std::move(left_operand) }, m_type{ type }, m_right_operand{ std::move(right_operand) } {}

void BinaryOperator::print_to_stream(std::ostream& os) const {
    auto operator_chars = [&] {
        // clang-format off
        switch (m_type) {
            using enum TokenType;
            case Plus:             return "+";
            case Minus:            return "-";
            case Asterisk:         return "*";
            case Slash:            return "/";
            default:               return "?";
        }
        // clang-format on
    }();
    os << '(' << *m_left_operand << ' ' << operator_chars << ' ' << *m_right_operand << ')';
}

void BinaryOperator::emit(std::vector<Instruction>& instructions) const {
    m_left_operand->emit(instructions);
    m_right_operand->emit(instructions);

    // clang-format off
    switch (m_type) {
        using enum TokenType;
        case Plus:     instructions.emplace_back(Add{}); break;
        case Minus:    instructions.emplace_back(Subtract{}); break;
        case Asterisk: instructions.emplace_back(Multiply{}); break;
        case Slash:    instructions.emplace_back(Divide{}); break;
        default:       break;
    }
    // clang-format on
}

Identifier::Identifier(char const name)
    : m_name{ name } {}

void Identifier::emit(std::vector<Instruction>& instructions) const {
    instructions.emplace_back(Parameter{ static_cast<usize>(m_name - 'x') });
}

void Identifier::print_to_stream(std::ostream& os) const {
    os << m_name;
}
