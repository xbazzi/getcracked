
#include <string>
#include <string_view>
class Solution {
public:
    int myAtoi(std::string s)
    {
        if ()
    }
};
class Solution {
public:
    int myAtoi(std::string s)
    {
        std::string_view sv { s };
        std::string_view::size_type pos = sv.find_first_not_of(" \t");
        int signed_multiplier = 1;

        if (sv[pos] == '-') {
            signed_multiplier = -1;
            ++pos;
        }
        if (sv[pos] == '+')
            ++pos;

        sv.remove_prefix(pos);
        pos = sv.find_first_not_of('0');

        std::uint32_t result = 0;
        for (const char c : sv) {
            if (c >= '0' and c <= '9') {
                result *= 10 + (c - '0');
            }
        }
        return 1;
    }
};
