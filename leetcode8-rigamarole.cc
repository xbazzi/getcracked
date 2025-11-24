#include <algorithm>
#include <limits>
#include <print>
#include <string>

class Solution {
public:
    int myAtoi(std::string s)
    {
        std::string::size_type n = s.size();
        std::string_view sv = s;

        bool is_signed { false };
        std::string_view::size_type bad_pos = sv.find_first_not_of(" \t123456789-");
        std::string_view::size_type sign_pos = sv.find("-");
        if (sign_pos != std::string_view::npos) {
            if (bad_pos < sign_pos)
                return 0;
            is_signed = true;
        }

        // bad_pos = sv.find_first_not_of(" \t0123456789-");
        std::string_view::size_type start = sv.find_first_of("123456789");
        if (bad_pos < start)
            return 0;
        if (start == std::string_view::npos)
            return 0;

        std::string_view::size_type end = sv.substr(start).find_first_not_of("123456789");
        if (end == std::string_view::npos)
            end = n;
        else
            end += start;

        if (start == end)
            return sv[start];
        unsigned long int num = 0;
        for (auto i { start }; i < end; ++i) {
            // if (num > static_cast<long>(std::numeric_limits<int>::max()))
            if (num > (1UL << 31UL) - 1UL) {
                num = 1 + static_cast<long>(std::numeric_limits<int>::max());
                break;
            }
            num = num * 10 + (sv[i] - '0');
        }
        return is_signed ? num * (-1) : num;
    }
};

#include <type_traits>
int main()
{

    using namespace std::literals;
    Solution sol;
    std::print("int: {}\n", sol.myAtoi("+1"s));
    // std::print("{}", std::numeric_limits<int>::max() >> 30);
    return EXIT_SUCCESS;
}