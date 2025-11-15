#include <stack>
#include <vector>

class MyQueue {
   public:
    using Stack = std::stack<int, std::vector<int>>;

    MyQueue() = default;

    void push(int x) { stack_.push(x); }

    int pop() {
        _move_to(stack_, tmp_stack_);
        auto tmp = tmp_stack_.top();
        tmp_stack_.pop();
        _move_to(tmp_stack_, stack_);
        return tmp;
    }

    int peek() {
        _move_to(stack_, tmp_stack_);
        auto tmp = tmp_stack_.top();
        _move_to(tmp_stack_, stack_);
        return tmp;
    }

    bool empty() { return stack_.empty(); }

   private:
    std::stack<int, std::vector<int>> stack_;
    std::stack<int, std::vector<int>> tmp_stack_;

    void _move_to(Stack& stack, Stack& other_stack) {
        while (!stack.empty())
        {
            other_stack.push(stack.top());
            stack.pop();
        }
    }
};

/**
 * Your MyQueue object will be instantiated and called as such:
 * MyQueue* obj = new MyQueue();
 * obj->push(x);
 * int param_2 = obj->pop();
 * int param_3 = obj->peek();
 * bool param_4 = obj->empty();
 */
