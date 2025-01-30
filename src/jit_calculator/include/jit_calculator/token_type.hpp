#pragma once

#include <lib2k/types.hpp>

enum class TokenType : usize {
    Plus,
    Minus,
    Asterisk,
    Slash,
    LeftParenthesis,
    RightParenthesis,
    Integer,
    Identifier,
    EndOfFile,
};
