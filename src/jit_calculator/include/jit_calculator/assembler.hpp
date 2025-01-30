#pragma once

#include <span>
#include <vector>
#include "instruction.hpp"

enum class CallingConvention {
    X86_64_SystemV,
};

template<CallingConvention calling_convention>
[[nodiscard]] std::vector<u8> assemble(std::span<Instruction const> instructions, usize arity);

// clang-format off
template<>
[[nodiscard]] std::vector<u8> assemble<CallingConvention::X86_64_SystemV>(
    std::span<Instruction const> instructions,
    usize arity
);
// clang-format on
