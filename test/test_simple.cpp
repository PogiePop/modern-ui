// Minimal test — verify toolchain works
#include <cstdio>
#include "core/Types.hpp"
using namespace ui;

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL: %s\n", #cond); failures++; } } while(0)

int main() {
    printf("=== Simple Type Tests ===\n");

    // Color
    Color c = Color::fromHex(0xFF3B82F6);
    CHECK(c.r > 0.2f && c.r < 0.3f);

    // Rect
    Rect r{0, 0, 100, 50};
    CHECK(r.right() == 110);
    CHECK(r.bottom() == 70);
    CHECK(r.contains({50, 25}));
    CHECK(!r.contains({150, 25}));

    // Point
    Point a{3, 4}, b{1, 2};
    Point sum = a + b;
    CHECK(sum.x == 4 && sum.y == 6);

    printf("=== Done: %d failures ===\n", failures);
    return failures;
}
