#include <atomic>
#include <cstdint>

namespace getcracked
{
    template <typename Element>
    class InstantWriteMultipleRead
    {
    public:
        InstantWriteMultipleRead() 
        {
        }

        void Write(const Element& value)
        {
            m_buf[1U - m_idx] = value;
            m_current.store(&m_buf[1U - m_idx], std::memory_order_release);
            m_idx = 1U - m_idx;
        }
        
        bool Read(Element& out) const
        {
            
            if (!m_current.load(std::memory_order_acquire))
                return false;
            out = *m_current.load(std::memory_order_acquire);
            return true;
        }

    private:
        using AlignedElement = alignas(64) Element;    
        AlignedElement m_buf[2]{};
        std::atomic<AlignedElement*> m_current{nullptr};
        std::uint8_t m_idx{0};
    };

} // namespace getcracked
