class Solution {
public:
    long long minEnd(int n, int x)
    {
        long long result = x;
        long long remaining = n - 1;
        long long bit_position = 1;
        
        while (remaining > 0) {
            // Find a bit position where x has a 0
            if ((x & bit_position) == 0) {
                // Set this bit in result based on the LSB of remaining
                if (remaining & 1) {
                    result |= bit_position;
                }
                remaining >>= 1;
            }
            bit_position <<= 1;
        }
        
        return result;
    }
};
