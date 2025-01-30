#pragma once

#include <string_view>
#include "assembler.hpp"
#include "function.hpp"
#include "lexer.hpp"
#include "parser.hpp"

template<CallingConvention calling_convention, typename Return, typename... Parameters>
[[nodiscard]] Function<Return(Parameters...)> jit(std::string_view const source) {
    auto const tokens = tokenize(source);
    auto const expression = parse(tokens);

    auto instructions = std::vector<Instruction>{};
    expression->emit(instructions);

    auto const binary = assemble<calling_convention>(instructions, sizeof...(Parameters));

    auto function = Function<Return(Parameters...)>{ binary };
    return function;
}
