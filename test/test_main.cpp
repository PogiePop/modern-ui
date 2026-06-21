// ModernUI — Unit Test Runner (single TU for simplicity)
// Compile and run this to execute all unit tests (no OpenGL window required)
#include "test_framework.hpp"

// Pull in all test modules via #include (single translation unit approach)
// This ensures all TEST() static registrations are included in the binary.
#include "test_types.cpp"
#include "test_theme.cpp"
#include "test_layout.cpp"
#include "test_widget.cpp"

int main() {
    return test::runAll();
}
