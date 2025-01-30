#include <cctype>
#include <jit_calculator/lexer.hpp>
#include <lib2k/types.hpp>
#include <stdexcept>

class Lexer final {
private:
    std::string_view m_source;
    usize m_index = 0;
    std::vector<Token> m_tokens;

public:
    [[nodiscard]] explicit Lexer(std::string_view const source)
        : m_source{ source } {}

    void tokenize() {
        while (not is_at_end()) {
            if (std::isspace(static_cast<unsigned char>(current()))) {
                advance();
                continue;
            }
            switch (current()) {
                case '+':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::Plus);
                    advance();
                    break;
                case '-':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::Minus);
                    advance();
                    break;
                case '*':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::Asterisk);
                    advance();
                    break;
                case '/':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::Slash);
                    advance();
                    break;
                case '(':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::LeftParenthesis);
                    advance();
                    break;
                case ')':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::RightParenthesis);
                    advance();
                    break;
                case 'x':
                case 'y':
                case 'z':
                    m_tokens.emplace_back(m_source.substr(m_index, 1), TokenType::Identifier);
                    advance();
                    break;
                default: {
                    if (not std::isdigit(static_cast<unsigned char>(current()))) {
                        throw std::runtime_error{ "Unexpected character" };
                    }
                    auto const start = m_index;
                    advance();
                    while (std::isdigit(static_cast<unsigned char>(current()))) {
                        advance();
                    }
                    m_tokens.emplace_back(m_source.substr(start, m_index - start), TokenType::Integer);
                    break;
                }
            }
        }
        m_tokens.emplace_back(std::string_view{}, TokenType::EndOfFile);
    }

    [[nodiscard]] std::vector<Token> take_tokens() & = delete;

    [[nodiscard]] std::vector<Token> take_tokens() && {
        return std::move(m_tokens);
    }

private:
    [[nodiscard]] bool is_at_end() const {
        return m_index >= m_source.size();
    }

    [[nodiscard]] char current() const {
        return is_at_end() ? '\0' : m_source.at(m_index);
    }

    void advance() {
        if (not is_at_end()) {
            ++m_index;
        }
    }
};

[[nodiscard]] std::vector<Token> tokenize(std::string_view const expression) {
    auto lexer = Lexer{ expression };
    lexer.tokenize();
    auto tokens = std::move(lexer).take_tokens();
    return tokens;
}
