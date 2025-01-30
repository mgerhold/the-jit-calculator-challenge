#include <jit_calculator/assembler.hpp>
#include "x86_x64_system_v_assembler.hpp"

// clang-format off
template<>
[[nodiscard]] std::vector<u8> assemble<CallingConvention::X86_64_SystemV>(
    std::span<Instruction const> const instructions,
    usize arity
) {
    // clang-format on
    auto assembler = X86X64SystemVAssembler{};
    assembler.assemble(instructions, arity);
    return std::move(assembler).take_binary();
}
