// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////
#include <type_traits>
#include <algorithm>

namespace getcracked {
    template <typename T>
    constexpr decltype(auto) move(T&& t) noexcept {
        return static_cast<std::remove_reference_t<T>&&>(t);
    }
}