#pragma once

#include <cassert>
#include <jit_calculator/instruction.hpp>
#include <lib2k/types.hpp>
#include <vector>

enum class Register : u8 {
    Rax = 0x0,
    Rcx = 0x1,
    Rdx = 0x2,
    Rbx = 0x3,
    Rsp = 0x4,
    Rbp = 0x5,
    Rsi = 0x6,
    Rdi = 0x7,
};

class X86X64SystemVAssembler final {
private:
    std::vector<u8> m_binary;
    usize m_stack_offset = 8;

public:
    void assemble(std::span<Instruction const> instructions, usize arity);
    [[nodiscard]] std::vector<u8> take_binary() & = delete;
    [[nodiscard]] std::vector<u8> take_binary() &&;

private:
    void assemble(Instruction const& instruction);
    void assemble(ImmediateValue const& immediate_value);
    void assemble(Add const& add);
    void assemble(Subtract const& subtract);
    void assemble(Multiply const& multiply);
    void assemble(Divide const& divide);
    void assemble(Parameter const& parameter);
    void push(Register source);
    void push(i64 immediate);
    void pop(Register target);

    void load_from_stack(usize offset, Register target);

    template<std::convertible_to<u8>... Bytes>
    void push_bytes(Bytes const... bytes) {
        assert(((bytes >= 0x00 && bytes <= 0xff) && ...));
        (m_binary.push_back(static_cast<u8>(bytes)), ...);
    }
};
