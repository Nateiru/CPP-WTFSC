// https://luyuhuang.tech/2022/10/30/lock-free-queue.html#
// need C++17 can't compile in C++20
#include <iostream>
#include <cstddef>          // 用于 size_t 类型
#include <atomic>           // 用于 atomic 类型
#include <memory>           // 用于 allocator 类型
#include <utility>          // 用于 std::forward 和 std::move

using std::allocator;
using std::atomic;

template <typename T, size_t Cap>
class spsc : private allocator<T> {
  T *data;
  atomic<size_t> head{0}, tail{0};
public:
  spsc(): data(allocator<T>::allocate(Cap)) {}
  spsc(const spsc &) = delete;
  spsc &operator=(const spsc &) = delete;
  spsc &operator=(const spsc &) volatile = delete;

  bool push(const T &val) {
    return emplace(val);
  }
  bool push(T &&val) {
    return emplace(std::move(val));
  }

  template <typename ...Args>
  bool emplace(Args &&...args) {  // 入队操作
    size_t t = tail.load(std::memory_order_relaxed);

    if ((t + 1) % Cap == head.load(std::memory_order_acquire)) // (1)
      return false;

    std::allocator<T>::construct(data + t, std::forward<Args>(args)...);
    // (2)  synchronizes-with (3)
    tail.store((t + 1) % Cap, std::memory_order_release); // (2)
    return true;
  }

  bool pop(T &val) { // 出队操作
    size_t h = head.load(std::memory_order_relaxed);

    if (h == tail.load(std::memory_order_acquire)) // (3)
      return false;

    val = std::move(data[h]);
    allocator<T>::destroy(data + h);
    // (4) synchronizes-with (1)
    head.store((h + 1) % Cap, std::memory_order_release); // (4)
    return true;
  }
};

int main() {
  spsc<int, 4> q;
  q.push(1);
  q.push(2);
  q.push(3);

  {
    int x;
    q.pop(x);
    std::cout << x << std::endl;
  }

  {
    int x;
    q.pop(x);
    std::cout << x << std::endl;
  }
  
  {
    int x;
    q.pop(x);
    std::cout << x << std::endl;
  }

  return 0;
}