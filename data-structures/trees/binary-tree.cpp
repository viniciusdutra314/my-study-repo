#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <cassert>

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
  Node<K,V>* search_key(K const& key){
      Node<K,V> * current=root;
      while (current){
          if (key==current->key){
              return current;
          }
          else if (key > current->key){
              current=current->right_child;
          }
          else{
              current=current->left_child;
          }
      }
      return nullptr;
  }
  Node<K,V>* search_max(){
      if (!root) return nullptr;
      Node<K,V>* current=root;
      while (current->right_child){
          current=current->right_child;
      }
      return current;
  }

  Node<K,V>* search_min(){
      if (!root) return nullptr;
      Node<K,V>* current=root;
      while (current->left_child){
          current=current->left_child;
      }
      return current;
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
    BinaryTree<int, std::string> btree;
    std::map<int, std::string> reference_map;
    std::vector<std::pair<int, std::string>> test_data = {
        {50, "fifty"}, {30, "thirty"}, {70, "seventy"},
        {20, "twenty"}, {40, "forty"}, {60, "sixty"}, {80, "eighty"}
    };

    for (const auto& [key,value] : test_data) {
        btree.add_node(key, value);
        reference_map[key] = value;
    }

    for (const auto& item : test_data) {
        Node<int, std::string>* node = btree.search_key(item.first);
        assert(node != nullptr);
        assert(node->value == reference_map[item.first]);
    }
    assert(btree.search_key(999) == nullptr);

    Node<int, std::string>* min_node = btree.search_min();
    assert(min_node != nullptr);
    assert(min_node->key == reference_map.begin()->first);
    assert(min_node->value == reference_map.begin()->second);

    Node<int, std::string>* max_node = btree.search_max();
    assert(max_node != nullptr);
    assert(max_node->key == reference_map.rbegin()->first);
    assert(max_node->value == reference_map.rbegin()->second);
    return 0;
}
