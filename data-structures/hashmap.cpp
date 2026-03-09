#include <algorithm>
#include <cctype>
#include <chrono>
#include <forward_list>
#include <fstream>
#include <print>
#include <string>
#include <unordered_map>
#include <vector>

template <typename K, typename V, typename HashFunc_t = std::hash<K>>
class HashMap {
private:
  size_t m_num_buckets;
  HashFunc_t m_hash_func;
  std::vector<std::forward_list<std::pair<K, V>>> m_buckets_array;

public:
  HashMap(size_t M) : m_num_buckets(M), m_buckets_array(M) {};
  V &operator[](K const &key) {
    auto &bucket = m_buckets_array[m_hash_func(key) % m_num_buckets];
    auto prev_it = bucket.before_begin();
    for (auto current_it = bucket.begin(); current_it != bucket.end();
         current_it++) {
      if (current_it->first == key) {
        return current_it->second;
      }
      prev_it = current_it;
    };
    return bucket.insert_after(prev_it, std::pair<K, V>(key, V{}))->second;
  }
  std::vector<std::pair<K, V>> get_all_key_value_pairs() {
    std::vector<std::pair<K, V>> result;
    for (auto &bucket : m_buckets_array) {
      for (auto &pair : bucket) {
        result.push_back(pair);
      }
    }
    return result;
  }
};

template <typename MapType>
void run_benchmark(std::string const &name, MapType &map) {
  std::ifstream text("bible.txt");
  auto start = std::chrono::high_resolution_clock::now();
  std::string word;
  while (text >> word) {
    std::erase_if(word, ::ispunct);
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    if (!word.empty()) {
      map[word] += 1;
    }
  }

  std::vector<std::pair<std::string, size_t>> freq_vec;
  if constexpr (requires { map.get_all_key_value_pairs(); }) {
    freq_vec = map.get_all_key_value_pairs();
  } else {
    for (const auto &pair : map) {
      freq_vec.push_back(pair);
    }
  }

  std::sort(freq_vec.begin(), freq_vec.end(),
            [](const auto &a, const auto &b) { return b.second < a.second; });

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;
  std::println("{} took: {} ms", name, duration.count());

  std::println("Top 10 words:");
  for (size_t i = 0; i < std::min<size_t>(10, freq_vec.size()); ++i) {
    std::println("{}: {}", freq_vec[i].first, freq_vec[i].second);
  }
}

int main() {
  auto hash_func = [](std::string const &x) {
    size_t sum = 0;
    for (auto c : x) {
      sum = 13 * sum + (unsigned char)c;
    }
    return sum;
  };

  size_t N=10'000;
  {
    HashMap<std::string, size_t, decltype(hash_func)> custom_map(N);
    run_benchmark("Custom HashMap", custom_map);
    std::println("bucket_size={}",N);
    std::println("");
  }

  {
    std::unordered_map<std::string, size_t> std_map;
    run_benchmark("std::unordered_map", std_map);
    std::println("bucket_size={}",std_map.bucket_count());
  }

  return 0;
}
