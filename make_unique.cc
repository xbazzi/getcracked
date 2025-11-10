// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

#include <concepts>
#include <memory>
#include <utility>

namespace getcracked
{
    template <class T>
    concept IsNotArray = !std::is_array_v<T>;
    // Implement here. Return std::unique_ptr<T>.

    template<IsNotArray T, class... Args>
    [[nodiscard]] auto make_unique(Args&&... args) 
        -> std::unique_ptr<T>
    {
        std::unique_ptr<T> result = std::unique_ptr<T>(new T{std::forward<Args>(args)...});
        return result;
    }
}

#include <iostream>
#include <cassert>
int main()
{
    std::unique_ptr<int> someint = getcracked::make_unique<int>(2);
    assert(*someint == 2);
    return 0;
}