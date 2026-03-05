#include <cassert>
#include <generator>
#include <iostream>
#include <stack>
#include <stdlib.h>
#include <variant>
#include <vector>

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

public:
  BinaryTree() { this->root = nullptr; };
  ~BinaryTree() {
    std::stack<Node<K, V> *> call_stack;
    if (this->root)
      call_stack.push(this->root);
    while (!call_stack.empty()) {
      Node<K, V> *current_node = call_stack.top();
      call_stack.pop();
      if (current_node->left_child)
        call_stack.push(current_node->left_child);
      if (current_node->right_child)
        call_stack.push(current_node->right_child);
      delete current_node;
    }
  }

  std::generator<Node<K, V> *> pre_order() {
    std::stack<Node<K, V> *> st;
    if (root)
      st.push(root);
    while (!st.empty()) {
      auto node = st.top();
      st.pop();
      co_yield node;
      if (node->right_child)
        st.push(node->right_child);
      if (node->left_child)
        st.push(node->left_child);
    }
  }

  std::generator<Node<K, V> *> in_order() {
    std::stack<Node<K, V> *> st;
    Node<K, V> *curr = root;
    while (!st.empty() || curr) {
      while (curr) {
        st.push(curr);
        curr = curr->left_child;
      }
      curr = st.top();
      st.pop();
      co_yield curr;
      curr = curr->right_child;
    }
  }





  std::generator<Node<K, V> *> pos_order() {
    std::stack<Node<K, V> *> st;
    Node<K, V> *curr = root;
    Node<K, V> *last_visited = nullptr;

    while (!st.empty() || curr) {
      if (curr) {
        st.push(curr);
        curr = curr->left_child;
      } else {
        Node<K, V> *peek_node = st.top();
        if (peek_node->right_child && last_visited != peek_node->right_child) {
          curr = peek_node->right_child;
        } else {
          co_yield peek_node;
          last_visited = peek_node;
          st.pop();
        }
      }
    }
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
  }
};

int main() {
  BinaryTree<int, std::monostate> tree;
  std::vector<int> keys = {45, 30, 60, 20, 35, 50, 70, 15, 25, 40, 55, 65, 75};

  for (const auto& key : keys) {
    tree.add_node(key, {});
  }

  auto verify = [](const std::string& name, auto generator, const std::vector<int>& expected) {
    std::cout << "Testing " << name << "... ";
    size_t i = 0;
    for (auto* node : generator) {
      assert(node->key == expected[i] && "Value mismatch");
      i++;
    }
    assert(i == expected.size() && "Too few elements in traversal");
    std::cout << "Passed!" << std::endl;
  };

  verify("Pre-order", tree.pre_order(), {45, 30, 20, 15, 25, 35, 40, 60, 50, 55, 70, 65, 75});
  verify("In-order", tree.in_order(), {15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75});
  verify("Post-order", tree.pos_order(), {15, 25, 20, 40, 35, 30, 55, 50, 65, 75, 70, 60, 45});
  return 0;
}
