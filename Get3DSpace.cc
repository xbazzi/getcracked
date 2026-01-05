// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////
// Implement GetSpace such that that it allocates a 3D array of ints of
// dimensions x, y, and z on the heap.

// Requirements
// Return an int***.
// You cannot use any data structures (such as vector).
// You must initialize all values to std::numeric_limits<int>::min() during
// allocation without a for-loop. Cracked developers prefer STL algorithms to
// raw loops. Consider edge-cases fully. Return nullptr when encountering
// invalid inputs.
//
// Example:
// auto*** arr = GetSpace(2, 3, 4);

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ranges>

int*** GetSpace(size_t x, size_t y, size_t z)
{
    if (x == 0 or y == 0 or z == 0)
        return nullptr;

    // Check for overflow
    if (x > SIZE_MAX / y or x * y > SIZE_MAX / z)
        return nullptr;

    auto arr = new int**[z];
    for (size_t j {}; j < z; ++j) {
        arr[j] = new int*[y];
        for (size_t k {}; k < y; ++k) {
            arr[j][k] = new int[x];
            std::ranges::fill(arr[j][k], arr[j][k] + x,
                std::numeric_limits<int>::min());
        }
    }

    return arr;
}

#include <iostream>
int main()
{
    int z = 10;
    int x = 10;
    int y = 10;
    auto space = GetSpace(x, y, z);
    std::cout << "these should all be negative max int\n";
    std::ranges::for_each(space[0][0], space[0][0] + z,
        [](auto el) { std::cout << el << '\n'; });
    // Properly deallocate all three dimensions
    for (size_t i {}; i < x; ++i) {
        for (size_t j {}; j < y; ++j) {
            delete[] space[i][j]; // Delete innermost (z dimension)
        }
        delete[] space[i]; // Delete middle layer (y dimension)
    }
    delete[] space; // Delete outermost layer (x dimension)
    return EXIT_SUCCESS;
}