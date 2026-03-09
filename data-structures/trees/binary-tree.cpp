#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <variant>
#include <vector>
#include <queue>

template <typename K, typename V> struct Node {
  K key;
  V value;
  Node *parent = nullptr;
  Node *left_child = nullptr;
  Node *right_child = nullptr;
};

template <typename K, typename V> class BinaryTree {
private:
  Node<K, V> *root;

  void destroy_recursive(Node<K, V> *node) {
    if (node) {
      destroy_recursive(node->left_child);
      destroy_recursive(node->right_child);
      delete node;
    }
  }

  void pre_order_recursive(Node<K, V> *node, std::vector<Node<K, V> *> &result) {
    if (node) {
      result.push_back(node);
      pre_order_recursive(node->left_child, result);
      pre_order_recursive(node->right_child, result);
    }
  }
  void in_order_recursive(Node<K, V> *node, std::vector<Node<K, V> *> &result) {
    if (node) {
      in_order_recursive(node->left_child, result);
      result.push_back(node);
      in_order_recursive(node->right_child, result);
    }
  }

  void post_order_recursive(Node<K, V> *node, std::vector<Node<K, V> *> &result) {
    if (node) {
      post_order_recursive(node->left_child, result);
      post_order_recursive(node->right_child, result);
      result.push_back(node);
    }
  }
  size_t height_recursive(Node<K,V>* node){
      if (!node) return 0;
      return 1+std::max(height_recursive(node->left_child),height_recursive(node->right_child));
  }
public:
  BinaryTree() { this->root = nullptr; };
  ~BinaryTree() {
    destroy_recursive(this->root);
  }

  std::vector<Node<K, V> *> pre_order() {
    std::vector<Node<K, V> *> result;
    pre_order_recursive(root, result);
    return result;
  }

  std::vector<Node<K, V> *> in_order() {
    std::vector<Node<K, V> *> result;
    in_order_recursive(root, result);
    return result;
  }

  std::vector<Node<K, V> *> pos_order() {
    std::vector<Node<K, V> *> result;
    post_order_recursive(root, result);
    return result;
  }

  void add_node(K const &key, V const &value) {
    if (!root) {
      root = new Node<K, V>;
      root->key = key;
      root->value = value;
      return;
    }
    Node<K, V> *current = nullptr;
    Node<K, V> *next = root;
    while (next) {
      current = next;
      next = (key > current->key) ? next->right_child : next->left_child;
    }
    auto *new_node = new Node<K, V>;
    if (key > current->key) {
      current->right_child = new_node;
    } else {
      current->left_child = new_node;
    }
    new_node->parent = current;
    new_node->value = value;
    new_node->key = key;
  }
  Node<K, V> *search_key(K const &key) {
    Node<K, V> *current = root;
    while (current) {
      if (key == current->key) {
        return current;
      } else if (key > current->key) {
        current = current->right_child;
      } else {
        current = current->left_child;
      }
    }
    return nullptr;
  }
  Node<K, V> *search_max() {
    if (!root)
      return nullptr;
    Node<K, V> *current = root;
    while (current->right_child) {
      current = current->right_child;
    }
    return current;
  }

  Node<K, V> *search_min() {
    if (!root)
      return nullptr;
    Node<K, V> *current = root;
    while (current->left_child) {
      current = current->left_child;
    }
    return current;
  };
  std::vector<Node<K,V>*> bfs(){
    std::queue<Node<K,V>*> queue;
    std::vector<Node<K,V>*> result;
    if (root) queue.push(root);
    while (!queue.empty()){
        auto node_ptr=queue.front();
        queue.pop();
        result.push_back(node_ptr);
        if (node_ptr->left_child) queue.push(node_ptr->left_child);
        if (node_ptr->right_child) queue.push(node_ptr->right_child);
    }
    return result;
  }
  size_t height(){
      return height_recursive(root);
  }
};

int main() {
  BinaryTree<int, std::monostate> tree;
  std::vector<int> keys = {45, 30, 60, 20, 35, 50, 70, 15, 25, 40, 55, 65, 75};

  for (const auto& key : keys) {
    tree.add_node(key, {});
  }
  assert(tree.height()==4);
  auto verify = [](const std::string& name, const std::vector<Node<int, std::monostate>*>& nodes, const std::vector<int>& expected) {
    std::cout << "Testing " << name << "... ";
    size_t i = 0;
    for (auto* node : nodes) {
      assert(node->key == expected[i] && "Value mismatch");
      i++;
    }
    assert(i == expected.size() && "Too few elements in traversal");
    std::cout << "Passed!" << std::endl;
  };

  verify("Pre-order", tree.pre_order(), {45, 30, 20, 15, 25, 35, 40, 60, 50, 55, 70, 65, 75});
  verify("In-order", tree.in_order(), {15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75});
  verify("Post-order", tree.pos_order(), {15, 25, 20, 40, 35, 30, 55, 50, 65, 75, 70, 60, 45});
  verify("BFS-order", tree.bfs(), {45, 30, 60, 20, 35, 50, 70, 15, 25, 40, 55, 65, 75});
  return 0;
}
