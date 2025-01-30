CPMAddPackage(
        NAME GTEST
        GITHUB_REPOSITORY google/googletest
        VERSION 1.15.2
        OPTIONS
        "INSTALL_GTEST OFF"
        "BUILD_SHARED_LIBS OFF"
)

add_executable(
        jit_calculator_tests
        jit_calculator_tests.cpp
)
target_link_libraries(
        jit_calculator_tests
        PRIVATE
        jit_calculator
)
target_link_system_libraries(jit_calculator_tests
        PRIVATE
        gtest_main
        gmock_main
)

include(GoogleTest)
gtest_discover_tests(jit_calculator_tests)
