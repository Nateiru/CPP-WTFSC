#include <atomic>
#include <memory>
#include <vector>
#include <thread>
#include <iostream>

template <typename T>
class LockFreeStack {
 public:
  void push(const T& data) {
    Node* new_node = new Node(data);
    new_node->next = head.load();
    while (!head.compare_exchange_weak(new_node->next, new_node)) {
      // compare_exchange_weak 在失败的时候会自动进行下面步骤：
      // new_node->next = head.load();
    }
  }

  std::shared_ptr<T> pop() {  // 还未考虑释放原来的头节点指针
    Node* old_head = head.load();
		// 考虑 head 不是 nullptr
    while (old_head && !head.compare_exchange_weak(old_head, old_head->next)) {
      old_head = head.load();
    }
    return old_head ? old_head->data : nullptr;
  }

 private:
  struct Node {
    std::shared_ptr<T> data;
    Node* next = nullptr;
    Node(const T& data_) : data(std::make_shared<T>(data_)) {}
  };

 private:
  std::atomic<Node*> head;
};

int main() {

  LockFreeStack<int> q;
  std::vector<std::thread> workers;
  int n = 100000;
  for (int i = 0 ; i < n; i++) {
    workers.emplace_back([&q, i](){
      q.push(i);
    });
  }
  for (auto &worker :workers) {
    worker.join();
  }

  for (int i = 0; i < n; i++)
    std::cout << *q.pop() << " \n"[i == n - 1];
  return 0;
}