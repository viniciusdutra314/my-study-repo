#include <cstdlib>
#include <iostream>
#include <stdlib.h>

template <typename K, typename V> struct Node {
  K key;
  V value;
  Node *parent;
  Node *left_child;
  Node *right_child;
};

template <typename K, typename V> class BinaryTree {
private:
  Node<K, V> *root;
public:
  BinaryTree() { this->root = nullptr; };
  void add_node(K const &key, V const &value) {
    if (!root) {
      root = (Node<K, V> *)malloc(sizeof(Node<K, V>));
      root->left_child = nullptr;
      root->right_child = nullptr;
      root->parent = nullptr;
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
    auto *new_node = (Node<K, V> *)malloc(sizeof(Node<K, V>));
    if (key > current->key) {
      current->right_child = new_node;
    } else {
      current->left_child = new_node;
    }
    new_node->parent = current;
    new_node->left_child = nullptr;
    new_node->right_child = nullptr;
    new_node->value = value;
    new_node->key = key;
  }
  void print_in_order_tree_walk(){
      in_order_tree_walk(root);
  }
  private:
    void in_order_tree_walk(Node<K,V> const * x){
        if (x){
            in_order_tree_walk(x->left_child);
            std::cout<<x->value<<std::endl;
            in_order_tree_walk(x->right_child);
        }
    }
};

int main() {
    BinaryTree<int, int> tree;
    tree.add_node(1, 1);
    tree.add_node(5, 5);
    tree.add_node(10, 10);
    tree.add_node(-1, -1);
    tree.print_in_order_tree_walk();

}
