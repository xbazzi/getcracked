// C++ Includes
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <format>
#include <functional>
#include <future>
#include <print>
#include <queue>
#include <ranges>
#include <thread>

namespace {
    thread_local int t_thread_id = -1;
}

class ThreadPool
{
  public:
    ThreadPool() = default;
    ThreadPool(std::size_t num_threads);

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;
    std::string get_thread_id();

  private:
    std::queue<std::move_only_function<void()>> m_tasks;
    std::vector<std::jthread> m_workers;
    mutable std::mutex m_mutex;
    std::counting_semaphore<> m_semaphore{0};  // C++20 semaphore
    std::atomic<std::size_t> m_active_tasks{0};
    std::atomic_bool m_stopping{false};
    // std::stop_source m_stop_source;  // C++20 stop token
};

ThreadPool::ThreadPool(std::size_t num_threads = std::thread::hardware_concurrency())
{
    using namespace std::chrono_literals;
    // auto stop_token = m_stop_source.get_token();   
    auto range = std::views::iota(0, static_cast<int>(num_threads));
    std::ranges::for_each(range, [this](int thread_id) {
        m_workers.emplace_back([this, thread_id](std::stop_token stoken) {
            while (!stoken.stop_requested() and !m_stopping.load(std::memory_order_acquire))
            {
                /// @todo Investigate how this blocks (spin? futex?)
                m_semaphore.acquire();
                
                /// @todo do we need the empty task check? we are doing it 
                /// in the scoped block anyway.
                if (stoken.stop_requested() && m_tasks.empty())
                    break;
                
                std::move_only_function<void()> task; 
                {
                    std::scoped_lock<std::mutex> lock(m_mutex);
                    if (m_tasks.empty())
                        continue;
                    task = std::move(m_tasks.front());
                    m_tasks.pop();

                }

                t_thread_id = thread_id;
                task();
            }
        });
    });
}

std::string ThreadPool::get_thread_id()
{
    return std::format("{}", t_thread_id);
}

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
    using Ret = std::invoke_result_t<F, Args...>;
    auto task = std::packaged_task<Ret()>(std::bind_front(std::forward<F>(f), std::forward<Args>(args)...));
    auto fut = task.get_future();
    {
        std::scoped_lock lock(m_mutex);
        m_tasks.emplace(std::move(task));
    }
    m_semaphore.release();
    return fut;
}

ThreadPool::~ThreadPool()
{

    std::ranges::for_each(m_workers, [this](auto& thread){ 
        m_semaphore.release();
        thread.request_stop();
    });
}

int main()
{
    using namespace std::chrono_literals;
    ThreadPool tp(4);
    
    // Task 1: void return - just wait for completion
    auto future_void = tp.enqueue([]() -> void {
        std::this_thread::sleep_for(100ms);
        std::print("Task 1 (void) completed on thread {}\n", t_thread_id);
    });
    
    // Task 2: int return - get the result
    auto future_int = tp.enqueue([](int x, int y) -> int {
        std::print("Task 2: computing {}+{} on thread {}\n", x, y, t_thread_id);
        return x + y;
    }, 10, 20);

    
    // Task 3: string return
    auto future_str = tp.enqueue([](std::string msg) -> std::string {
        std::print("Task 3: processing '{}' on thread {}\n", msg, t_thread_id);
        return msg + " - processed!";
    }, "Hello");

    auto future_loop = tp.enqueue([](std::size_t n = 0UL) -> int {
        std::print("Task 4: Computing fat loop on thread {}\n",  t_thread_id);
        while (n < (1 << 30))
        {
            (++n, --n);
            ++n;
        }
        return true;
    }, 0);
    
    // Wait for void task (no return value to capture)
    future_void.get();
    std::print("✓ Void task finished\n");
    
    // Get results from non-void tasks
    std::print("✓ Result 2: {}\n", future_int.get());
    std::print("✓ Result 3: {}\n", future_str.get());
    std::print("✓ Result 4: {}\n", future_loop.get());
    
    return EXIT_SUCCESS;
}