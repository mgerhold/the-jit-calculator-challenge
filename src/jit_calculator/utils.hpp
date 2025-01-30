#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <jit_calculator/expression.hpp>
#include <jit_calculator/precedence.hpp>
#include <utility>

template<typename T, T... values>
struct Max {
    static constexpr auto value = std::max({ values... });
};

template<typename T, T... values>
static constexpr auto max_value = Max<T, values...>::value;

template<typename T, T... values>
static constexpr auto max_underlying_value = max_value<std::underlying_type_t<T>, std::to_underlying(values)...>;

class Parser;

using PrefixParserFunction = std::unique_ptr<Expression> (Parser::*)();
using InfixParserFunction = std::unique_ptr<Expression> (Parser::*)(std::unique_ptr<Expression> left_operand);

template<auto key_value>
struct ParserTableEntry {
    static constexpr auto key = key_value;
    PrefixParserFunction prefix_parser;
    InfixParserFunction infix_parser;
    Precedence infix_precedence;
};

template<auto value>
constexpr auto parser_table_entry_value(ParserTableEntry<value>) {
    return value;
}

template<typename T, typename... Values>
constexpr auto max_parser_table_entry_value() {
    return max_underlying_value<TokenType, Values::key...>;
}

struct ParserTableRecord {
    PrefixParserFunction prefix_parser;
    InfixParserFunction infix_parser;
    Precedence infix_precedence;
};

template<typename... Entries>
auto create_parser_table(Entries... entries) {
    auto result = std::array<ParserTableRecord, max_parser_table_entry_value<TokenType, Entries...>() + 1>();
    std::fill(result.begin(), result.end(), ParserTableRecord{ nullptr, nullptr, Precedence::Unknown });
    (([&] {
         result.at(std::to_underlying(Entries::key)) = ParserTableRecord{
             .prefix_parser{ entries.prefix_parser },
             .infix_parser{ entries.infix_parser },
             .infix_precedence{ entries.infix_precedence },
         };
     })(),
     ...);
    return result;
}
