#pragma once

#include <string_view>
#include "token_type.hpp"

struct Token final {
    std::string_view lexeme;
    TokenType type;

    [[nodiscard]] Token(std::string_view const lexeme, TokenType const type)
        : lexeme{ lexeme }, type{ type } {}
};
