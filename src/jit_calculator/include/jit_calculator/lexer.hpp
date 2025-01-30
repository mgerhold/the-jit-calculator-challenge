#pragma once

#include <vector>
#include "token.hpp"

[[nodiscard]] std::vector<Token> tokenize(std::string_view expression);
