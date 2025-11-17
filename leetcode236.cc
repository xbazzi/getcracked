#include <cstddef>
#include <cstdlib>
#include <memory>
#include <print>

// Definition for a binary tree node.
struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL)
    {
    }
};

class Solution
{
  public:
    TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q)
    {
        if (!root or p == root or q == root)
            return root;
        TreeNode *left = lowestCommonAncestor(root->left, p, q);
        TreeNode *right = lowestCommonAncestor(root->right, p, q);
        if (left && right)
            return root;
        return left ? left : right;
    }
};

int main()
{
    std::unique_ptr<TreeNode> node1 = std::make_unique<TreeNode>(1);
    std::unique_ptr<TreeNode> node2 = std::make_unique<TreeNode>(2);
    std::unique_ptr<TreeNode> node3 = std::make_unique<TreeNode>(3);
    std::unique_ptr<TreeNode> node_stray = std::make_unique<TreeNode>(0);
    node1->left = node2.get();
    node1->right = node3.get();
    Solution sol{};
    TreeNode *result = sol.lowestCommonAncestor(node1.get(), node2.get(), node_stray.get());
    if (!result)
    {
        std::print("u done goofed\n");
        return EXIT_FAILURE;
    }
    std::print("result->val: {}", result->val);

    return EXIT_SUCCESS;
}