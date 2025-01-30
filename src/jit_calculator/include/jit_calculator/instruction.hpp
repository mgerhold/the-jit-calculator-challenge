#pragma once

#include <iostream>
#include <lib2k/types.hpp>
#include <optional>
#include <variant>

struct ImmediateValue final {
    i64 value;

    friend std::ostream& operator<<(std::ostream& os, ImmediateValue const& immediate_value) {
        return os << "ImmediateValue[" << immediate_value.value << "]";
    }
};

struct Add final {
    friend std::ostream& operator<<(std::ostream& os, Add const&) {
        return os << "Add";
    }
};

struct Subtract final {
    friend std::ostream& operator<<(std::ostream& os, Subtract const&) {
        return os << "Subtract";
    }
};

struct Multiply final {
    friend std::ostream& operator<<(std::ostream& os, Multiply const&) {
        return os << "Multiply";
    }
};

struct Divide final {
    friend std::ostream& operator<<(std::ostream& os, Divide const&) {
        return os << "Divide";
    }
};

struct Parameter final {
    usize index;

    [[nodiscard]] explicit Parameter(usize const index)
        : index{ index } {}

    friend std::ostream& operator<<(std::ostream& os, Parameter const& parameter) {
        return os << "Parameter[" << parameter.index << "]";
    }
};

using Instruction = std::variant<ImmediateValue, Add, Subtract, Multiply, Divide, Parameter>;
