#pragma once

#include "expression.hpp"
#include "lexer.hpp"

[[nodiscard]] std::unique_ptr<Expression> parse(std::span<Token const> tokens);
