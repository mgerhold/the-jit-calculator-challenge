#include <gtest/gtest.h>
#include <jit_calculator/lexer.hpp>

using namespace std::string_view_literals;

TEST(LexerTests, TokenizeExpressions) {
    auto const a = tokenize(""sv);
    EXPECT_EQ(a.size(), 1);
    EXPECT_EQ(a.at(0).type, TokenType::EndOfFile);

    auto const b = tokenize("         \t\n\v\r       "sv);
    EXPECT_EQ(b.size(), 1);
    EXPECT_EQ(b.at(0).type, TokenType::EndOfFile);

    auto const c = tokenize("    42    "sv);
    EXPECT_EQ(c.size(), 2);
    EXPECT_EQ(c.at(0).type, TokenType::Integer);
    EXPECT_EQ(c.at(0).lexeme, "42"sv);
    EXPECT_EQ(c.at(1).type, TokenType::EndOfFile);

    auto const d = tokenize("1+2+-*/(-3---+5)xyz"sv);
    EXPECT_EQ(d.size(), 20);
    EXPECT_EQ(d.at(0).type, TokenType::Integer);
    EXPECT_EQ(d.at(0).lexeme, "1"sv);
    EXPECT_EQ(d.at(1).type, TokenType::Plus);
    EXPECT_EQ(d.at(2).type, TokenType::Integer);
    EXPECT_EQ(d.at(2).lexeme, "2"sv);
    EXPECT_EQ(d.at(3).type, TokenType::Plus);
    EXPECT_EQ(d.at(4).type, TokenType::Minus);
    EXPECT_EQ(d.at(5).type, TokenType::Asterisk);
    EXPECT_EQ(d.at(6).type, TokenType::Slash);
    EXPECT_EQ(d.at(7).type, TokenType::LeftParenthesis);
    EXPECT_EQ(d.at(8).type, TokenType::Minus);
    EXPECT_EQ(d.at(9).type, TokenType::Integer);
    EXPECT_EQ(d.at(9).lexeme, "3"sv);
    EXPECT_EQ(d.at(10).type, TokenType::Minus);
    EXPECT_EQ(d.at(11).type, TokenType::Minus);
    EXPECT_EQ(d.at(12).type, TokenType::Minus);
    EXPECT_EQ(d.at(13).type, TokenType::Plus);
    EXPECT_EQ(d.at(14).type, TokenType::Integer);
    EXPECT_EQ(d.at(14).lexeme, "5"sv);
    EXPECT_EQ(d.at(15).type, TokenType::RightParenthesis);
    EXPECT_EQ(d.at(16).type, TokenType::Identifier);
    EXPECT_EQ(d.at(16).lexeme, "x"sv);
    EXPECT_EQ(d.at(17).type, TokenType::Identifier);
    EXPECT_EQ(d.at(17).lexeme, "y"sv);
    EXPECT_EQ(d.at(18).type, TokenType::Identifier);
    EXPECT_EQ(d.at(18).lexeme, "z"sv);
    EXPECT_EQ(d.at(19).type, TokenType::EndOfFile);
}
