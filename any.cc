// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// DO NOT DELETE OR CHANGE ANYTHING UNTIL class any //
//////////////////////////////////////////////////////

/**
 * @file any.cc
 * @author Xander Bazzi (codemaster@xbazzi.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-08
 * 
 * 
 */


#include <memory>
#include <concepts>
#include <type_traits>
#include <variant>
#include <new>
#include <typeinfo>


namespace getcracked {

    template <typename T>
    struct is_small_trait {
        constexpr static inline bool value = false;
    };

    template <>
    struct is_small_trait<int> {
        constexpr static inline bool value = true;
    };

    template <typename T>
    constexpr bool is_small_v = is_small_trait<T>::value;


    /**
     * @brief Type-erased, type-safe container
     * @todo Stop using template class like a n00b
     */
    class any
    {
    private:
        static constexpr std::uint64_t SMALL_SIZE = sizeof(int);
        using Storage = alignas(alignof(int)) std::byte[sizeof(int)];

        Storage storage_;
        AnyType* ptr_{nullptr};  // Points to heap allocation or nullptr for small objects
        bool on_heap{false};
    public:
        any() = default;

        any(const any&) = delete;
        any& operator=(const any&) = delete;
        any(any&&) = delete;
        any& operator=(any&&) = delete;

        // Implement a constructor that takes in an object by copy.
        any(const AnyType& other)
        {
            if constexpr (sizeof(AnyType) <= SMALL_SIZE && alignof(AnyType) <= alignof(int))
            {
                std::construct_at(reinterpret_cast<AnyType*>(&storage_), other);
                on_heap = false;
                ptr_ = nullptr;
            } else {
                ptr_ = reinterpret_cast<AnyType*>(::operator new(sizeof(AnyType)));
                // ptr_ = static_cast<AnyType*>(new (mem) AnyType{other});
                std::construct_at(ptr_, other);
                on_heap = true;
            }
        }

        ~any() {
            if (on_heap && ptr_) {
                std::destroy_at(ptr_);
                ::operator delete(ptr_);
            } else if (!on_heap) {
                std::destroy_at(reinterpret_cast<AnyType*>(&storage_));
            }
        }


        template <typename T>
        const T& any_cast() const {
            if (!std::is_same_v<T, AnyType>)
            {
                throw std::bad_cast("U done goofed");
            }
            // static_assert(std::is_same_v<T, AnyType>, "Type mismatch in any_cast");

            if (on_heap) {
                return *ptr_;
            } else {
                return *std::launder(reinterpret_cast<const AnyType*>(&storage_));
            }
        }


    };
    // template <typename AnyType>
    // any(const AnyType&) -> any<AnyType>;
}

#include <cassert>
#include <iostream>
#include <vector>
#include <complex>
#include <any>

struct MyStruct
{
    int a;
    int b;
    int c;
    int d;
    double e;
    double f;
    double g;
} somestruct;

int main()
{
    getcracked::any<int> someAny{1};
    std::cout << "sizeof(any<int>): " << sizeof(someAny) << "\n";

    const int& value = someAny.any_cast<int>();
    std::cout << "Value stored: " << value << "\n";

    getcracked::any<double> anotherAny{3.14};
    std::cout << "sizeof(any<double>): " << sizeof(anotherAny) << "\n";
    std::cout << "Double value: " << anotherAny.any_cast<double>() << "\n";

    return 0;
}