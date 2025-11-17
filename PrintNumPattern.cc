// Write a recursive function PrintNumPattern() to output the following number pattern:
//
// Given a positive integer as input (Ex: 12), subtract another positive integer (Ex: 3) continually until a negative
// value is reached, and then continually add the second integer until the first integer is again reached. For this lab,
// do not end output with a newline.
//
// Example:
//
// Input:
// 12
// 3
//
// Output:
// 12 9 6 3 0 -3 0 3 6 9 12
//

#include <cstdlib>
#include <print>
#include <stack>

void PrintNumPattern(int num1, int num2)
{
    if (num1 < 0)
    {
        std::print("{} ", num1);
        return;
    }
    std::print("{} ", num1);
    PrintNumPattern(num1 - num2, num2);
    std::print("{} ", num1);
}

void PrintNumPatternStack(int num1, int num2)
{
    std::stack<int> stack;
    stack.push(num1);
    bool coming_up{false};
    while (!stack.empty())
    {
        if (stack.top() < 0)
            coming_up = true;

        if (!coming_up)
        {
            std::print("{} ", stack.top());
            num1 -= num2;
            stack.push(num1);
        }
        else
        {
            std::print("{} ", stack.top());
            stack.pop();
        }
    }
}

int main()
{
    int num1{12};
    int num2{3};

    PrintNumPatternStack(num1, num2);

    return EXIT_SUCCESS;
}