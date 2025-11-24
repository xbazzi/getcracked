#include <vector>

using namespace std;
class Solution {
public:
    vector<vector<int>> floodFill(vector<vector<int>>& image, int sr, int sc, int color)
    {
        if (!image.size())
            return { {} };
        if (!image[0].size())
            return { {} };

        helper(image, sr, sc, image[sr][sc], color);
        return image;
    }

    void helper(std::vector<std::vector<int>>& image, int i, int j, int val, int color)
    {
        // dilation algorithm
        if (i == image.size() or j == image[0].size() or j < 0 or i < 0 or image[i][j] != val or image[i][j] == color)
            return;
        image[i][j] = color;
        helper(image, i + 1, j, val, color);
        helper(image, i - 1, j, val, color);
        helper(image, i, j + 1, val, color);
        helper(image, i, j - 1, val, color);
    }
};
