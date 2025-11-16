
// Definition for a binary tree node.
struct TreeNode
{
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* left, TreeNode* right)
        : val(x), left(left), right(right) {}
};

class Solution {
   public:
    bool isSubtree(TreeNode* root, TreeNode* subRoot) {
        if (!root) return false;
        if (is_same_tree(root, subRoot)) return true;
        return isSubtree(root->left, subRoot) or
               isSubtree(root->right, subRoot);
    }

   private:
    bool is_same_tree(TreeNode* root, TreeNode* subRoot) {
        if (not root and not subRoot) return true;
        if (not root or not subRoot) return false;
        if (root->val != subRoot->val) return false;
        return is_same_tree(root->right, subRoot->right) and
               is_same_tree(root->left, subRoot->left);
    }
};
