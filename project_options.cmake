include(${PROJECT_SOURCE_DIR}/cmake/warnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake)

# the following function was taken from:
# https://github.com/cpp-best-practices/cmake_template/blob/main/ProjectOptions.cmake
macro(check_sanitizer_support)
    if ((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
        set(supports_ubsan ON)
    else ()
        set(supports_ubsan OFF)
    endif ()

    if ((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
        set(supports_asan OFF)
    else ()
        set(supports_asan ON)
    endif ()
endmacro()

if (PROJECT_IS_TOP_LEVEL)
    option(jit_calculator_warnings_as_errors "Treat warnings as errors" ON)
    option(jit_calculator_enable_undefined_behavior_sanitizer "Enable undefined behavior sanitizer" ${supports_ubsan})
    option(jit_calculator_enable_address_sanitizer "Enable address sanitizer" ${supports_asan})
    option(jit_calculator_build_tests "Build unit tests" ON)
else ()
    option(jit_calculator_warnings_as_errors "Treat warnings as errors" OFF)
    option(jit_calculator_enable_undefined_behavior_sanitizer "Enable undefined behavior sanitizer" OFF)
    option(jit_calculator_enable_address_sanitizer "Enable address sanitizer" OFF)
    option(jit_calculator_build_tests "Build unit tests" OFF)
endif ()

add_library(jit_calculator_warnings INTERFACE)
jit_calculator_set_warnings(jit_calculator_warnings ${jit_calculator_warnings_as_errors})

add_library(jit_calculator_sanitizers INTERFACE)
jit_calculator_enable_sanitizers(jit_calculator_sanitizers ${jit_calculator_enable_address_sanitizer} ${jit_calculator_enable_undefined_behavior_sanitizer})

add_library(jit_calculator_project_options INTERFACE)
target_link_libraries(jit_calculator_project_options
        INTERFACE jit_calculator_warnings
        INTERFACE jit_calculator_sanitizers
)
