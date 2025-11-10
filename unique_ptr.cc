// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace getcracked {
template <typename T>
struct custom_deleter {
  void operator()(T* pointer) const { delete pointer; }
};

template <typename T, typename custom_deleter = custom_deleter<T>>
class unique_ptr : private custom_deleter {
  static_assert(!(std::is_array_v<T> || std::is_bounded_array_v<T>),
                "dem arrays is forbidden 💅 sir");

 private:
  T* p_data;

 public:
  unique_ptr() : p_data{nullptr} {}
  unique_ptr(T* pointer) : p_data{pointer} {}

  unique_ptr(const unique_ptr&) {
    throw std::logic_error("u trippin tryna copy a unique ptr lmao");
  }
  unique_ptr& operator=(const unique_ptr&) {
    throw std::logic_error("good one bro");
  }

  unique_ptr(unique_ptr&& other) noexcept : p_data{other.p_data} {
    p_data = other.p_data;
    other.p_data = nullptr;
  }

  unique_ptr& operator=(unique_ptr&& other) noexcept {
    if (this != &other) {
      this->reset();
      this->p_data = other.p_data;
      other.p_data = nullptr;
    }
    return *this;
  }
  ~unique_ptr() { reset(); }

  T* release() {
    auto tmp = p_data;
    p_data = nullptr;
    return tmp;
  }

  void reset(T* pointer = nullptr) {
    if (this != nullptr && p_data) this->operator()(p_data);
    p_data = pointer;
  }

  bool is_owning() const { return this->operator bool(); }
  T& operator*() const { return *p_data; }
  T* operator->() const { return p_data; }
  explicit operator bool() const { return p_data != nullptr; }
};
}  // namespace getcracked
#include <iostream>
using namespace std;

// int main()
// {

//     struct MyDeleter
//     {
//         void operator()(int* t)
//         {
//             return;
//         }
//     };

//     int x = 2;
//     getcracked::unique_ptr<int> intptr(new int(x));
//     assert(*intptr == x);
//     return 0;
// }