#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

template <typename T, typename Allocator_t = std::allocator<T>> class Stack {
private:
  size_t m_top_index = 0;
  size_t m_capacity = 0;
  T *m_arr_ptr = nullptr;
  [[no_unique_address]]
  Allocator_t m_allocator;

public:
  Stack() = default;
  Stack(size_t pre_allocated_size) {
    m_top_index = 0;
    m_capacity = pre_allocated_size;
    m_arr_ptr = m_capacity > 0 ? m_allocator.allocate(m_capacity) : nullptr;
  }
  Stack(Stack const &other) {
    m_allocator = other.m_allocator;
    m_capacity = other.m_capacity;
    m_top_index = other.m_top_index;
    m_arr_ptr = m_capacity > 0 ? m_allocator.allocate(m_capacity) : nullptr;
    for (size_t i = 0; i < m_top_index; i++) {
      std::construct_at(m_arr_ptr + i, other.m_arr_ptr[i]);
    }
  }
  Stack(Stack &&other) noexcept {
    m_allocator = std::move(other.m_allocator);
    m_capacity = other.m_capacity;
    m_top_index = other.m_top_index;
    m_arr_ptr = other.m_arr_ptr;

    other.m_arr_ptr = nullptr;
    other.m_capacity = other.m_top_index = 0;
  }
  Stack &operator=(Stack const &other) {
    if (this != &other) {
      Stack<T, Allocator_t> tmp_stack(other);
      std::swap(m_arr_ptr, tmp_stack.m_arr_ptr);
      std::swap(m_capacity, tmp_stack.m_capacity);
      std::swap(m_top_index, tmp_stack.m_top_index);
      std::swap(m_allocator, tmp_stack.m_allocator);
    }
    return *this;
  }
  Stack &operator=(Stack &&other) noexcept {
    if (this != &other) {
      Stack<T, Allocator_t> tmp_stack(std::move(other));
      std::swap(m_arr_ptr, tmp_stack.m_arr_ptr);
      std::swap(m_capacity, tmp_stack.m_capacity);
      std::swap(m_top_index, tmp_stack.m_top_index);
      std::swap(m_allocator, tmp_stack.m_allocator);
    }
    return *this;
  }

  ~Stack() {
    while (!this->empty()) {
      this->pop();
    }
    m_allocator.deallocate(m_arr_ptr, m_capacity);
    m_top_index = m_capacity = 0;
  }

  bool empty() const { return m_top_index == 0; }
  size_t size() const { return m_top_index; }
  T &top() {
    assert(!(this->empty()));
    return m_arr_ptr[m_top_index - 1];
  }
  void pop() {
    assert(!(empty()));
    std::destroy_at(m_arr_ptr + (--m_top_index));
  }

  void push(T const &elem) {
    if (m_top_index == m_capacity) {
      size_t new_capacity = m_capacity == 0 ? 1 : 2 * m_capacity;
      T *new_allocation = m_allocator.allocate(new_capacity);
      for (size_t i = 0; i < m_top_index; i++) {
        std::construct_at(new_allocation + i, std::move(m_arr_ptr[i]));
        std::destroy_at(m_arr_ptr + i);
      }
      m_allocator.deallocate(m_arr_ptr, m_capacity);
      m_arr_ptr = new_allocation;
      m_capacity = new_capacity;
    }
    std::construct_at(m_arr_ptr + m_top_index++, elem);
  }
};

int main() {
  // Test: Default Constructor, empty(), size()
  {
    Stack<int> s;
    assert(s.empty());
    assert(s.size() == 0);
  }

  // Test: Pre-allocated size constructor, push(), top()
  {
    Stack<int> s(10);
    s.push(42);
    assert(!s.empty());
    assert(s.size() == 1);
    assert(s.top() == 42);
  }

  // Test: pop()
  {
    Stack<int> s;
    s.push(1);
    s.push(2);
    assert(s.top() == 2);
    s.pop();
    assert(s.top() == 1);
    s.pop();
    assert(s.empty());
  }

  // Test: Dynamic resizing (pushing beyond initial capacity)
  {
    Stack<int> s(1);
    s.push(10);
    s.push(20); // Should trigger resize
    assert(s.size() == 2);
    assert(s.top() == 20);
    s.pop();
    assert(s.top() == 10);
  }

  // Test: Copy Constructor
  {
    Stack<std::string> s1;
    s1.push("hello");
    s1.push("world");
    Stack<std::string> s2(s1);
    assert(s2.size() == 2);
    assert(s2.top() == "world");
    s2.pop();
    assert(s2.top() == "hello");
    assert(s1.size() == 2); // original unchanged
  }

  // Test: Move Constructor
  {
    Stack<int> s1;
    s1.push(100);
    Stack<int> s2(std::move(s1));
    assert(s2.size() == 1);
    assert(s2.top() == 100);
    assert(s1.empty());
  }

  // Test: Copy Assignment Operator
  {
    Stack<int> s1;
    s1.push(1);
    Stack<int> s2;
    s2.push(2);
    s2.push(3);
    s1 = s2;
    assert(s1.size() == 2);
    assert(s1.top() == 3);
  }

  // Test: Move Assignment Operator
  {
    Stack<int> s1;
    s1.push(5);
    Stack<int> s2;
    s2.push(10);
    s1 = std::move(s2);
    assert(s1.size() == 1);
    assert(s1.top() == 10);
    assert(s2.empty());
  }

  // Test: Complex types (std::string) to check memory management
  {
    Stack<std::string> s;
    s.push(std::string("first"));
    s.push(std::string("second"));
    assert(s.top() == "second");
    s.pop();
    assert(s.top() == "first");
  }

  std::cout << "All tests passed successfully!" << std::endl;
  return 0;
}
