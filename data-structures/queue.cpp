#include <cassert>
#include <memory>
#include <optional>
#include <string>

template <typename T> class Queue {
private:
  T *m_ring_buffer = nullptr;
  size_t m_ring_capacity = 0;
  size_t m_head = 0;
  size_t m_size = 0;

public:
  Queue() = default;
  Queue(size_t capacity) {
    m_ring_buffer = new T[capacity];
    m_ring_capacity = capacity;
  };
  ~Queue() { delete[] m_ring_buffer; }
  void push_back(T const &element) {
    if (m_size == m_ring_capacity) {
      size_t new_capacity = m_ring_capacity != 0 ? m_ring_capacity * 2 : 1;
      T *new_buffer = new T[new_capacity];
      for (size_t i = 0; i < m_size; i++) {
        std::construct_at(
            new_buffer + i,
            std::move(m_ring_buffer[(m_head + i) % m_ring_capacity]));
        std::destroy_at(m_ring_buffer + (m_head + i) % m_ring_capacity);
      }
      delete[] m_ring_buffer;
      m_ring_buffer = new_buffer;
      m_head = 0;
      m_ring_capacity = new_capacity;
    }
    m_ring_buffer[(m_head + m_size) % m_ring_capacity] = element;
    m_size++;
  }
  std::optional<T> pop() {
    if (m_size != 0) {
      m_size--;
      T element = m_ring_buffer[m_head];
      m_head = (m_head + 1) % m_ring_capacity;
      return element;
    } else {
      return std::nullopt;
    }
  }
};

int main() {
  // Test default constructor
  {
    Queue<std::string> q;
    assert(q.pop() == std::nullopt);
  }

  // Test capacity constructor
  {
    Queue<std::string> q(2);
    q.push_back("a");
    q.push_back("b");
    assert(q.pop().value() == "a");
    assert(q.pop().value() == "b");
    assert(q.pop() == std::nullopt);
  }

  // Test push_back with resizing (triggers memory management logic)
  {
    Queue<std::string> q;
    q.push_back("1");
    q.push_back("2");
    q.push_back("3"); // Should trigger resize
    assert(q.pop().value() == "1");
    assert(q.pop().value() == "2");
    assert(q.pop().value() == "3");
    assert(q.pop() == std::nullopt);
  }

  // Test wrap-around behavior and resizing
  {
    Queue<std::string> q(2);
    q.push_back("first");
    q.push_back("second");
    q.pop(); // head moves to 1
    q.push_back("third"); // head is 1, size is 2, capacity is 2. Wraps around.

    // Trigger resize while head is not 0
    q.push_back("fourth");

    assert(q.pop().value() == "second");
    assert(q.pop().value() == "third");
    assert(q.pop().value() == "fourth");
    assert(q.pop() == std::nullopt);
  }

  // Final check with a loop
  {
    Queue<std::string> q;
    for (int i = 0; i < 100; ++i) {
      q.push_back(std::to_string(i));
    }
    for (int i = 0; i < 100; ++i) {
      assert(q.pop().value() == std::to_string(i));
    }
  }

  return 0;
}
