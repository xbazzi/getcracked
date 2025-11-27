// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <new>
#include <stdexcept>
#include <thread>
#include <type_traits>

namespace getcracked {
class ReaderWriterLock {
public:
    unsigned ReaderLock()
    {
        // Writers take priority. Spin wait
        while (m_write_seqlock & 1) {
            std::this_thread::yield();
        }
        m_reading.store(true, std::memory_order_release);
    }

    unsigned ReaderUnlock()
    {
    }

    void WriterLock()
    {
        while (m_write_seqlock & 1) {
            for (int i {}; i < 64; ++i) {
                std::this_thread::yield();
            }
        }
        ++m_write_seqlock;
    }

    void WriterUnlock()
    {
    }

private:
    std::atomic<std::uint64_t> m_write_seqlock {};
    std::atomic<std::uint64_t> m_read_seqlock {};
    std::atomic<std::uint64_t> m_reading {};
};
}