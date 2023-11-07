// https://jbseg.medium.com/lock-free-queues-e48de693654b

#include <atomic>
#include <memory>
#include <vector>
#include <thread>
#include <iostream>


template<typename T>
class LockFreeQueue {
private:
  struct Node {
    std::shared_ptr<T> data;
    Node * next;
    Node() : Node(T()) {}
    Node(T const &data_):
      data(std::make_shared<T>(data_))
    {}
  };
  std::atomic<Node *> head;
  std::atomic<Node *> tail;
public:
  LockFreeQueue() : head(new Node), tail(head.load()) {}
  void push(T const &data) {
    auto new_data = std::make_shared<T>(data);
    std::atomic<Node *> new_node = new Node;
    Node *old_tail = tail.load();
    old_tail->data.swap(new_data);
    old_tail->next = std::move(new_node);
    tail.store(new_node);
  }

  std::shared_ptr<T> pop() {
    Node* old_head = pop_head();
    if (old_head == nullptr) {
      return nullptr;
    }
    const std::shared_ptr<T> res(old_head->data);  // 2
    delete old_head;
    return res;
  }

private:
  Node* pop_head() {
    Node* old_head = head.load();
    if (old_head == tail.load()) {  // 1
      return nullptr;
    }
    head.store(old_head->next);
    return old_head;
  }
};

int main() {

  LockFreeQueue<int> q;

  std::vector<std::thread> workers;

  int n = 10;

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