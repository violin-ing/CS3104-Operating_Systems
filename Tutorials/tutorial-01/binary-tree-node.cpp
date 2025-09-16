#include <iostream>

template <typename T>
class TreeNode {
public:
    T val;
    TreeNode* left;
    TreeNode* right;

    // Constructor
    TreeNode(T value) : val(value), left(nullptr), right(nullptr) {}
};


// Usage:
// TreeNode<int>* root = new TreeNode<int>(69);
// TreeNode<std::string>* root = new TreeNode<std::string>("poo");