// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

namespace getcracked 
{
    template <typename T>
    struct custom_deleter
    {
        void operator()(T* pointer) const
        {
            delete pointer;
        }
    };

    template <typename T, typename custom_deleter = custom_deleter<T>>
    class unique_ptr
    {

    private:
        T* m_data;
    public:
        unique_ptr() : m_data{nullptr} {}
        unique_ptr(T* pointer) : m_data{pointer}

        unique_ptr(const unique_ptr&) {
            throw std::logic_error("u trippin tryna copy a unique ptr n shi lmao");
         };
        unique_ptr& operator=(const unique_ptr&) {
            throw std::logic_error("good one bro");
         };

        unique_ptr(unique_ptr&& other) noexcept
            : m_data{other.m_data}
        {
            other.m_data = nullptr;
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            if (this != other)
            {
                this->reset(nullptr);
                this->m_data = other.m_data;
                other.reset(nullptr);
            }
            return *this;
        }

        ~unique_ptr()
        {

        }

        T* release()
        {

        }

        void reset(T* pointer)
        {

        }

        bool is_owning() const { }


        T& operator*() const { }
        T* operator->() const { }
        operator bool() const { }

    };
}