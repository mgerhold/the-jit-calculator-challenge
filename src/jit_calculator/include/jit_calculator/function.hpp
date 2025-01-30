#pragma once

#include <sys/mman.h>
#include <cstring>
#include <iomanip>
#include <lib2k/types.hpp>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>

template<typename Return, typename... Parameters>
class Function;

template<typename Return, typename... Parameters>
class Function<Return(Parameters...)> final {
private:
    struct Deleter final {
        usize size;

        [[nodiscard]] explicit Deleter(usize const size)
            : size{ size } {}

        void operator()(void* ptr) const {
            if (ptr != MAP_FAILED) {
                munmap(ptr, size);
            }
        }
    };

    std::unique_ptr<void, Deleter> m_buffer;

public:
    [[nodiscard]] explicit Function(std::span<u8 const> const binary)
        : m_buffer{
              mmap(nullptr, binary.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
              Deleter{ binary.size() },
          } {
        if (m_buffer.get() == MAP_FAILED) {
            throw std::bad_alloc{};
        }
        std::memcpy(m_buffer.get(), binary.data(), binary.size());
    }

    Return operator()(Parameters... parameters) const {
        auto const function =
            reinterpret_cast<Return (*)(Parameters...)>(reinterpret_cast<std::intptr_t>(m_buffer.get()));
        return function(parameters...);
    }

    friend std::ostream& operator<<(std::ostream& ostream, Function const& function) {
        ostream << std::setfill('0') << std::hex;
        for (auto i = usize{ 0 }; i < function.m_buffer.get_deleter().size; ++i) {
            ostream << std::setw(2) << static_cast<u32>(reinterpret_cast<u8*>(function.m_buffer.get())[i]) << ' ';
        }
        return ostream;
    }
};
