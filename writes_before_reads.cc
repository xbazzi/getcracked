// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <new>
#include <stdexcept>
#include <thread>
#include <type_traits>

namespace getcracked {
class ReaderWriterLock {
public:
    unsigned ReaderLock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv_read.wait(lock, [&]() { return !(m_writer_count or m_waiting_writers); });
        return m_reader_count++;
    }

    unsigned ReaderUnlock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        --m_reader_count;
        auto remaining = m_reader_count;
        if (!m_reader_count)
            m_cv_write.notify_one();

        return remaining;
    }

    void WriterLock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_waiting_writers;
        m_cv_write.wait(lock, [&]() { return !m_writer_count and !m_reader_count; });
        ++m_waiting_writers;
        m_writer_count = 1;
    }

    void WriterUnlock()
    {
        std::unique_lock lock(m_mutex);
        m_writer_count = 0;
        if (m_waiting_writers > 0)
            m_cv_write.notify_one();
        else
            m_cv_read.notify_all();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv_read;
    std::condition_variable m_cv_write;

    std::uint32_t m_reader_count;
    std::uint32_t m_writer_count;
    std::uint32_t m_waiting_writers;
};
}
#include <print>

int main()
{
    getcracked::ReaderWriterLock lock {};
    unsigned int readers {}, writers {};
    std::println("hi mom");
    lock.ReaderLock();
    std::println("Reader locked, total: {}", ++readers);
    lock.ReaderLock();
    std::println("Reader locked, total: {} ", ++readers);
    lock.ReaderUnlock();
    std::println("Reader unlocked, total: {} ", --readers);
    lock.ReaderUnlock();
    std::println("Reader unlocked, total: {} ", --readers);
    lock.WriterLock();
    std::println("Writer locked, total: {}", ++writers);
    lock.WriterUnlock();
    lock.WriterLock();
    std::println("locked thrice");

    return EXIT_SUCCESS;
}