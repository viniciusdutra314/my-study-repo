#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <variant>
#include <vector>

template <typename K, typename V> struct Node {
  K key;
  [[no_unique_address]]
  V value;
  Node *parent = nullptr;
  Node *left_child = nullptr;
  Node *right_child = nullptr;
};

template <typename K, typename V> class BinarySearchTree {
private:
  Node<K, V> *root;

  static void destroy_recursive(Node<K, V> *node) {
    if (node) {
      destroy_recursive(node->left_child);
      destroy_recursive(node->right_child);
      delete node;
    }
  }

  static void pre_order_recursive(Node<K, V> *node,
                           std::vector<Node<K, V> *> &result) {
    if (node) {
      result.push_back(node);
      pre_order_recursive(node->left_child, result);
      pre_order_recursive(node->right_child, result);
    }
  }
  static void in_order_recursive(Node<K, V> *node, std::vector<Node<K, V> *> &result) {
    if (node) {
      in_order_recursive(node->left_child, result);
      result.push_back(node);
      in_order_recursive(node->right_child, result);
    }
  }

 static void post_order_recursive(Node<K, V> *node,
                            std::vector<Node<K, V> *> &result) {
    if (node) {
      post_order_recursive(node->left_child, result);
      post_order_recursive(node->right_child, result);
      result.push_back(node);
    }
  }
  size_t height_recursive(Node<K, V> *node) {
    if (!node)
      return 0;
    return 1 + std::max(height_recursive(node->left_child),
                        height_recursive(node->right_child));
  }

public:
  BinarySearchTree() { this->root = nullptr; };
  ~BinarySearchTree() { destroy_recursive(this->root); }

  std::vector<Node<K, V> *> pre_order() const {
    std::vector<Node<K, V> *> result;
    pre_order_recursive(root, result);
    return result;
  }

  std::vector<Node<K, V> *> in_order() const {
    std::vector<Node<K, V> *> result;
    in_order_recursive(root, result);
    return result;
  }

  std::vector<Node<K, V> *> post_order() const {
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
  Node<K, V> *search_key(K const &key) const {
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
  Node<K, V> *search_max() const {
    if (!root)
      return nullptr;
    Node<K, V> *current = root;
    while (current->right_child) {
      current = current->right_child;
    }
    return current;
  }

  Node<K, V> *search_min() const {
    if (!root)
      return nullptr;
    Node<K, V> *current = root;
    while (current->left_child) {
      current = current->left_child;
    }
    return current;
  };
  std::vector<Node<K, V> *> bfs() const {
    std::queue<Node<K, V> *> queue;
    std::vector<Node<K, V> *> result;
    if (root)
      queue.push(root);
    while (!queue.empty()) {
      auto node_ptr = queue.front();
      queue.pop();
      result.push_back(node_ptr);
      if (node_ptr->left_child)
        queue.push(node_ptr->left_child);
      if (node_ptr->right_child)
        queue.push(node_ptr->right_child);
    }
    return result;
  };

  Node<K, V> *get_successor(Node<K, V> *node) const {
    if (node == nullptr) {
      return nullptr;
    }

    if (node->right_child) {
      Node<K, V> *current = node->right_child;
      while (current->left_child) {
        current = current->left_child;
      }
      return current;
    } else {
      Node<K, V> *current = node;
      Node<K, V> *p = node->parent;
      while (p and current == p->right_child) {
        current = p;
        p = p->parent;
      }
      return p;
    }
  }

  Node<K, V> *get_predecessor(Node<K, V> *node) const {
    if (node->left_child) {
      Node<K, V> *current = node->left_child;
      while (current->right_child) {
        current = current->right_child;
      }
      return current;
    } else {
      Node<K, V> *current = node;
      Node<K, V> *p = current->parent;
      while (p and current == p->left_child) {
        current = p;
        p = p->parent;
      }
      return p;
    }
  };

  void remove(K const &key) {
    Node<K, V> *node = this->search_key(key);
    if (!node) {
      return;
    }
    Node<K, V> *parent = node->parent;
    bool is_right_child = parent != nullptr and parent->right_child == node;

    if (node->left_child and node->right_child) {
      Node<K, V> *successor = this->get_successor(node);
      assert(successor != nullptr);
      assert(successor->left_child == nullptr);
      node->key = successor->key;
      node->value = successor->value;
      Node<K, V> *succ_parent = successor->parent;
      Node<K, V> *succ_child = successor->right_child;
      if (successor == succ_parent->right_child) {
        succ_parent->right_child = succ_child;
      } else {
        succ_parent->left_child = succ_child;
      }
      if (succ_child){
          succ_child->parent = succ_parent;
      }
      delete successor;
      return;
    } else if ((node->right_child == nullptr) !=
               (node->left_child == nullptr)) {
      Node<K, V> *child =
          node->left_child != nullptr ? node->left_child : node->right_child;
      if (parent==nullptr){
          this->root=child;
          child->parent=nullptr;
      }else{
          child->parent = parent;
          if (is_right_child) {
            parent->right_child = child;
          } else {
            parent->left_child = child;
          }
      }
      delete node;
      return;

    } else if (!node->right_child and !node->left_child) {
      if (parent == nullptr) {
        this->root = nullptr;
      } else {
        if (is_right_child) {
          parent->right_child = nullptr;
        } else {
          parent->left_child = nullptr;
        }
      }
      delete node;
      return;
    }
  }

  size_t height() { return height_recursive(root); }
};

template <typename K>
using Set = BinarySearchTree<K,std::monostate>;


int main() {
  auto verify_keys = [](const std::string &name, Set<int> &t,
                        const std::vector<int> &expected_in_order) {
    std::cout << "Testing " << name << "... ";
    auto nodes = t.in_order();
    assert(nodes.size() == expected_in_order.size());
    for (size_t i = 0; i < expected_in_order.size(); i++) {
      assert(nodes[i]->key == expected_in_order[i]);
    }
    std::cout << "Passed!" << std::endl;
  };

  auto build_base_tree = []() {
    Set<int> t;
    std::vector<int> base = {45, 30, 60, 20, 35, 50, 70, 15, 25, 40, 55, 65, 75};
    for (int k : base) t.add_node(k, {});
    return t;
  };

  // remove leaf
  {
    auto t = build_base_tree();
    t.remove(15);
    verify_keys("remove leaf (15)", t, {20,25,30,35,40,45,50,55,60,65,70,75});
    assert(t.search_key(15) == nullptr);
  }

  // remove node with one child (50 has only right child 55 in this tree)
  {
    auto t = build_base_tree();
    t.remove(50);
    verify_keys("remove one-child node (50)", t, {15,20,25,30,35,40,45,55,60,65,70,75});
    assert(t.search_key(50) == nullptr);
    assert(t.search_key(55) != nullptr);
  }

  // remove node with two children (30 has children 20 and 35)
  {
    auto t = build_base_tree();
    t.remove(30);
    verify_keys("remove two-children node (30)", t, {15,20,25,35,40,45,50,55,60,65,70,75});
    assert(t.search_key(30) == nullptr);
  }

  // remove root with two children
  {
    auto t = build_base_tree();
    t.remove(45);
    verify_keys("remove root with two children (45)", t, {15,20,25,30,35,40,50,55,60,65,70,75});
    assert(t.search_key(45) == nullptr);
  }

  // remove missing key should not change tree
  {
    auto t = build_base_tree();
    t.remove(999);
    verify_keys("remove missing key (999)", t, {15,20,25,30,35,40,45,50,55,60,65,70,75});
  }

  // single-node tree: remove root leaf
  {
    Set<int> t;
    t.add_node(10, {});
    t.remove(10);
    verify_keys("remove only node (10)", t, {});
    assert(t.search_key(10) == nullptr);
  }

  std::cout << "All remove() tests passed\n";
  return 0;
}
