#include <chrono>
#include <iostream>
#include <jit_calculator/jit_calculator.hpp>
#include <string>

int main() {
    std::cout << "Enter an expression: ";
    auto expression = std::string{};  // e.g. "x + (3 - y) * 2"
    std::getline(std::cin, expression);

    auto function = std::optional<Function<i64(i64, i64, i64)>>();
    try {
        auto const start = std::chrono::high_resolution_clock::now();
        function = jit<CallingConvention::X86_64_SystemV, i64, i64, i64, i64>(expression);
        auto const elapsed = std::chrono::high_resolution_clock::now() - start;
        auto const nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
        std::cout << "Expression has been JIT-compiled to machine code in " << nanoseconds.count() << " ns.\n"
                  << function.value() << '\n';
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    while (true) {
        std::cout << "\tx = ";
        auto x = i64{};
        std::cin >> x;
        std::cout << "\ty = ";
        auto y = i64{};
        std::cin >> y;
        std::cout << "\tz = ";
        auto z = i64{};
        std::cin >> z;

        auto const start = std::chrono::high_resolution_clock::now();
        auto const result = function.value()(x, y, z);
        auto const elapsed = std::chrono::high_resolution_clock::now() - start;
        auto const nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
        std::cout << "\tResult calculated in " << nanoseconds.count() << " ns: " << std::dec << result << "\n\n";
    }
}
