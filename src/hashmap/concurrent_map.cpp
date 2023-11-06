#include <list>         // std::list
#include <mutex>        // std::unique_lock
#include <vector>       // std::vector
#include <memory>       // std::unique_ptr
#include <string>       // std::string
#include <iostream>     // std::cout
#include <functional>   // std::hash
#include <shared_mutex> // std::shared_mutex

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentMap {
 public:
  // 桶数默认为 19（一般用 x % 桶数作为 x 的桶索引，桶数为质数可使桶分布均匀）
  ConcurrentMap(std::size_t n = 19, const Hash& h = Hash{})
      : buckets_(n), hasher_(h) {
    // 哈希分桶
    for (auto& bucket : buckets_) {
      bucket.reset(new Bucket);
    }
  }
  ConcurrentMap(ConcurrentMap&&) = default; 
  ConcurrentMap& operator=(ConcurrentMap&&) = default; 

  ConcurrentMap(const ConcurrentMap&) = delete;
  ConcurrentMap& operator=(const ConcurrentMap&) = delete;

  V get(const K& k, const V& default_value = V{}) const {
    return get_bucket(k).get(k, default_value);
  }

  void set(const K& k, const V& v) { get_bucket(k).set(k, v); }

  void erase(const K& k) { get_bucket(k).erase(k); }

 private:
  struct Bucket {
    std::list<std::pair<K, V>> data;
    mutable std::shared_mutex m;  // 每个桶都用这个锁保护

    V get(const K& k, const V& default_value) const {
      // 没有修改任何值，异常安全
      std::shared_lock<std::shared_mutex> l(m);  // 只读锁，可共享
      auto it = std::find_if(data.begin(), data.end(),
                             [&](auto& x) { return x.first == k; });
      return it == data.end() ? default_value : it->second;
    }

    void set(const K& k, const V& v) {
      std::unique_lock<std::shared_mutex> l(m);  // 写，单独占用
      auto it = std::find_if(data.begin(), data.end(),
                             [&](auto& x) { return x.first == k; });
      if (it == data.end()) {
        data.emplace_back(k, v);  // emplace_back 异常安全
      } else {
        it->second = v;  // 赋值可能抛异常，但值是用户提供的，可放心让用户处理
      }
    }

    void erase(const K& k) {
      std::unique_lock<std::shared_mutex> l(m);  // 写，单独占用
      auto it = std::find_if(data.begin(), data.end(),
                             [&](auto& x) { return x.first == k; });
      if (it != data.end()) {
        data.erase(it);
      }
    }
  };

  Bucket& get_bucket(const K& k) const {  // 桶数固定因此可以无锁调用
    return *buckets_[hasher_(k) % buckets_.size()];
  }

  std::vector<std::unique_ptr<Bucket>> buckets_;
  Hash hasher_;
};

int main() {

  ConcurrentMap<std::string, int> mp;
  mp.set("zhuziyi", 2023110868);
  std::cout << mp.get("zhuziyi") << std::endl;
  return 0;
}
