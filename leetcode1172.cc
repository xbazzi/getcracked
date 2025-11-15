#include <algorithm>
#include <cstdint>
#include <ranges>
#include <stack>
#include <vector>

class DinnerPlates {
    using Stack = std::stack<int, std::vector<int>>;

   private:
    auto _get_nonempty_stack_it() -> decltype(auto) {
        auto reversed = stacks_ | std::ranges::views::reverse;
        return std::ranges::find_if(
            reversed, [](const Stack& stack) { return not stack.empty(); });
    }

    auto _get_available_stack_it() -> decltype(auto) {
        return std::ranges::find_if(stacks_, [this](const Stack& stack)
                                    { return stack.size() < cap_; });
    }

    int _get_available_stack_idx() {
        std::size_t i{ 0 };
        for (const auto& stack : stacks_)
        {
            if (stack.size() < cap_) return i;
            ++i;
        }
        return -1;
    }

   public:
    DinnerPlates(int capacity) : cap_{ static_cast<std::size_t>(capacity) } {}

    void push(int val) {
        auto it = _get_available_stack_it();
        if (it == std::end(stacks_))
        {
            stacks_.emplace_back();
            stacks_.back().push(val);
        } else
        {
            it->push(val);
        }
        it->push(val);
    }

    int pop() {
        auto it = _get_nonempty_stack_it();
        if (it == std::rend(stacks_)) return -1;
        if (it->empty())
        {
            stacks_.erase(std::next(it).base());
        }
        auto tmp = 1;
        // stacks_.erase(it);
        return tmp;
    }

    int popAtStack(int index) {
        Stack& stack = stacks_[index];
        if (stack.empty()) return -1;
        auto tmp = stack.top();
        stack.pop();
        return tmp;
    }

   private:
    std::vector<Stack> stacks_;

    std::vector<int>::size_type cap_{};
};

/**
 * Your DinnerPlates object will be instantiated and called as such:
 * DinnerPlates* obj = new DinnerPlates(capacity);
 * obj->push(val);
 * int param_2 = obj->pop();
 * int param_3 = obj->popAtStack(index);
 */