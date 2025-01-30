#include "x86_x64_system_v_assembler.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <utility>

template<std::integral T>
[[nodiscard]] std::array<u8, sizeof(T)> to_little_endian(T value) {
    if constexpr (std::endian::native != std::endian::little) {
        value = std::byteswap(value);
    }
    auto result = std::array<u8, sizeof(T)>{};
    std::memcpy(result.data(), &value, sizeof(T));
    return result;
}

void X86X64SystemVAssembler::assemble(std::span<Instruction const> const instructions, usize const arity) {
    // The stack offset is 8 bytes because the return address is pushed onto the stack.
    // Make room for the parameters.
    m_stack_offset += 8 * arity;

    // push rbp
    push_bytes(0x55);

    // mov rbp,rsp
    push_bytes(0x48, 0x89, 0xe5);

    if (arity > 0) {
        push_bytes(0x48, 0x89, 0x7d, 0xf8); // mov QWORD PTR [rbp-0x8],rdi
    }
    if (arity > 1) {
        push_bytes(0x48, 0x89, 0x75, 0xf0); // mov QWORD PTR [rbp-0x10],rsi
    }
    if (arity > 2) {
        push_bytes(0x48, 0x89, 0x55, 0xe8); // mov QWORD PTR [rbp-0x18],rdx
    }
    if (arity > 3) {
        throw std::runtime_error{ "Too many parameters." };
    }

    for (auto const& instruction : instructions) {
        assemble(instruction);
    }

    // pop rbp
    push_bytes(0x5d);

    // ret
    push_bytes(0xc3);
}

[[nodiscard]] std::vector<u8> X86X64SystemVAssembler::take_binary() && {
    return std::move(m_binary);
}

void X86X64SystemVAssembler::assemble(Instruction const& instruction) {
    std::visit([&](auto const& instr) { assemble(instr); }, instruction);
}

void X86X64SystemVAssembler::assemble(ImmediateValue const& immediate_value) {
    push(immediate_value.value);
}

void X86X64SystemVAssembler::assemble(Add const&) {
    pop(Register::Rdx);
    pop(Register::Rax);
    push_bytes(0x48, 0x01, 0xd0);  // add rax, rdx
    push(Register::Rax);
}

void X86X64SystemVAssembler::assemble(Subtract const&) {
    pop(Register::Rdx);
    pop(Register::Rax);
    push_bytes(0x48, 0x29, 0xd0);  // sub rax, rdx
    push(Register::Rax);
}

void X86X64SystemVAssembler::assemble(Multiply const&) {
    pop(Register::Rdx);
    pop(Register::Rax);
    push_bytes(0x48, 0x0f, 0xaf, 0xc2);  // imul rax, rdx
    push(Register::Rax);
}

void X86X64SystemVAssembler::assemble(Divide const&) {
    pop(Register::Rcx);
    pop(Register::Rax);
    push_bytes(0x48, 0x99, 0x48, 0xf7, 0xf9);  // cqo, idiv rcx
    push(Register::Rax);
}

void X86X64SystemVAssembler::assemble(Parameter const& parameter) {
    load_from_stack((parameter.index + 1) * 8, Register::Rax);
    push(Register::Rax);
}

void X86X64SystemVAssembler::push(Register const source) {
    if (not(source >= Register::Rax and source <= Register::Rdi)) {
        throw std::runtime_error{ "Invalid register." };
    }
    if (m_stack_offset <= std::numeric_limits<u8>::max()) {
        auto const mod_rm_byte = 0b01000101 | (std::to_underlying(source) << 3);
        push_bytes(0x48, 0x89, mod_rm_byte, static_cast<u8>(-static_cast<i16>(m_stack_offset & 0xff)));
    } else if (m_stack_offset <= std::numeric_limits<u32>::max()) {
        auto const mod_rm_byte = 0b10000101 | (std::to_underlying(source) << 3);
        push_bytes(0x48, 0x89, mod_rm_byte);
        for (auto const byte : to_little_endian(static_cast<u32>(-static_cast<i64>(m_stack_offset & 0xffffffff)))) {
            m_binary.push_back(byte);
        }
    } else {
        throw std::runtime_error{ "Stack offset too large." };
    }

    m_stack_offset += 8;
}

void X86X64SystemVAssembler::push(i64 const immediate) {
    // Moving a 64 bit immediate into the stack directly is not possible. We need to
    // move the immediate into a register and then move the register into the stack.

    // Load immediate into rax: `mov rax, imm64`
    push_bytes(0x48, 0xb8);  // REX prefix (64-bit), opcode (b8 + rax)
    for (auto const byte : to_little_endian(immediate)) {
        m_binary.push_back(byte);
    }

    push(Register::Rax);
}

void X86X64SystemVAssembler::pop(Register const target) {
    m_stack_offset -= 8;

    load_from_stack(m_stack_offset, target);
}

void X86X64SystemVAssembler::load_from_stack(usize const offset, Register target) {
    if (not(target >= Register::Rax and target <= Register::Rdi)) {
        throw std::runtime_error{ "Invalid register." };
    }
    if (offset <= std::numeric_limits<u8>::max()) {
        auto const mod_rm_byte = 0b01000101 | (std::to_underlying(target) << 3);
        push_bytes(0x48, 0x8b, mod_rm_byte, static_cast<u8>(-static_cast<i16>(offset & 0xff)));
    } else if (offset <= std::numeric_limits<u32>::max()) {
        auto const mod_rm_byte = 0b10000101 | (std::to_underlying(target) << 3);
        push_bytes(0x48, 0x8b, mod_rm_byte);
        for (auto const byte : to_little_endian(static_cast<u32>(-static_cast<i64>(offset & 0xffffffff)))) {
            m_binary.push_back(byte);
        }
    } else {
        throw std::runtime_error{ "Stack offset too large." };
    }
}
