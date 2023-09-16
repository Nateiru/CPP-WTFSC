// https://github.com/downdemo/Cpp-Concurrency-in-Action-2ed/blob/master/docs/05_designing_lock_based_concurrent_data_structure.md

#include <memory>   // unique_ptr and shared_ptr
#include <iostream> // std::cout 
#include <thread>   // std::thread
#include <cassert>  // std::static_assert
#include <condition_variable> // condition_variable

// 普通的链表实现队列
// 头节点和尾节点指向队头和队尾
// 队列是空的时候 head_ = tail_ = nullptr
// 尾插法 每次 pop 头节点
template <typename T> 
class Queue {
public:
  Queue() = default;
  Queue(Queue&&) = default;
  Queue& operator=(Queue&&) = delete; 
  
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete; 

  void push(T x) {
    auto new_node = std::make_unique<Node>(x);
    Node* new_tail_node = new_node.get();
    if (tail_) {
      tail_->next = std::move(new_node);
    }
    else {
      // 说明队列是空
      head_ = std::move(new_node);
    }
    tail_ = new_tail_node;
  }

  std::shared_ptr<T> try_pop() {
    // 说明队列是空
    if (!head_) {
      return nullptr;
    }
    // head 中的数据全部被 move
    auto ret = std::make_shared<T>(std::move(head_->v));
    head_ = std::move(head_->next);
    return ret;
  }

private:
  struct Node {
    explicit Node(T x) : v(std::move(x)) {};
    T v;
    std::unique_ptr<Node> next;
  };

  std::unique_ptr<Node> head_;
  Node *tail_{nullptr};
};

// 上面代码 push 的时候会同时访问 head_ 和 tail_
// 头节点前初始化一个 dummy 节点即可，这样 push 只访问尾节点
// 队列为空时 head_ == tail_
// tail_ 指向的是队尾的下一个节点，是一个 dummy 节点
template <typename T> 
class DummyQueue {
public:
  DummyQueue() : head_(new Node), tail_(head_.get()) {}
  DummyQueue(DummyQueue&&) = default;
  DummyQueue& operator=(DummyQueue&&) = delete; 
  
  DummyQueue(const DummyQueue&) = delete;
  DummyQueue& operator=(const DummyQueue&) = delete; 

  void push(T x) {
    auto new_val = std::make_shared<T>(std::move(x));
    auto new_node = std::make_unique<Node>();
    Node* new_tail_node = new_node.get();
    tail_->v = std::move(new_val);
    tail_->next = std::move(new_node); 
    tail_ = new_tail_node;
  }

  std::shared_ptr<T> try_pop() {
    // 说明队列是空
    if (head_.get() == tail_) {
      return nullptr;
    }
    // head 中的数据全部被 move
    std::shared_ptr<T> ret = head_->v;
    head_ = std::move(head_->next);
    return ret;
  }

private:
  struct Node {
    std::shared_ptr<T> v;
    std::unique_ptr<Node> next;
  };

  std::unique_ptr<Node> head_;
  Node *tail_{nullptr};
};

// 基于 DummyQueue 实现线程安全的 ConcurrentQueue
// 锁用于互斥、条件变量用于同步通知
template <typename T> 
class ConcurrentQueue {
public:
  ConcurrentQueue() : head_(new Node), tail_(head_.get()) {}
  ConcurrentQueue(ConcurrentQueue&&) = default;
  ConcurrentQueue& operator=(ConcurrentQueue&&) = delete; 
  
  ConcurrentQueue(const ConcurrentQueue&) = delete;
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; 

  void push(T x) {
    auto new_val = std::make_shared<T>(std::move(x));
    auto new_node = std::make_unique<Node>();
    Node* new_tail_node = new_node.get();
    {
      std::lock_guard<std::mutex> l(tail_mutex_);
      tail_->v = std::move(new_val);
      tail_->next = std::move(new_node); 
      tail_ = new_tail_node;
    }
    cv_.notify_one();
  }

  std::shared_ptr<T> try_pop() {
    std::unique_ptr<Node> head_node = try_pop_head();
    return head_node ? head_node->v : nullptr;
  }

  bool empty() {
    std::lock_guard<std::mutex> l(head_mutex_);
    return head_.get() == get_tail();
  }

  std::shared_ptr<T> wait_and_pop() {
    std::unique_ptr<Node> head_node = wait_pop_head();
    return head_node->v;
  }

private:
  struct Node {
    std::shared_ptr<T> v;
    std::unique_ptr<Node> next;
  };

  std::unique_ptr<Node> try_pop_head() {
    std::lock_guard<std::mutex> l(head_mutex_);
    if (head_.get() == get_tail()) {
      return nullptr;
    }
    return pop_head();
  }

  std::unique_ptr<Node> wait_pop_head() {
    std::unique_lock<std::mutex> l(wait_for_data());
    return pop_head();
  }

  std::unique_lock<std::mutex> wait_for_data() {
    std::unique_lock<std::mutex> l(head_mutex_);
    cv_.wait(l, [this] { return head_.get() != get_tail(); });
    return l;
  }

  std::unique_ptr<Node> pop_head() {
    std::unique_ptr<Node> head_node = std::move(head_);
    head_ = std::move(head_node->next);
    return head_node;
  }

  Node* get_tail() {
    std::lock_guard<std::mutex> l(tail_mutex_);
    return tail_;
  }

  std::unique_ptr<Node> head_;
  Node *tail_{nullptr};
  std::mutex head_mutex_;
  std::mutex tail_mutex_;
  std::condition_variable cv_;
};

int main() {
  {
    /// test Queue
    std::cout << "============ BEGIN TEST QUEUE ============" << std::endl;
    Queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    std::cout << *q.try_pop() << std::endl;
    std::cout << *q.try_pop() << std::endl;
    std::cout << *q.try_pop() << std::endl;
    std::cout << "============= END TEST QUEUE =============" << std::endl;
  }
  std::cout << std::endl;
  {
    /// test DummyQueue
    std::cout << "============ BEGIN TEST DUMMYQUEUE ============" << std::endl;
    DummyQueue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    std::cout << *q.try_pop() << std::endl;
    std::cout << *q.try_pop() << std::endl;
    std::cout << *q.try_pop() << std::endl;
    std::cout << "============= END TEST DUMMYQUEUE =============" << std::endl;
  }
  std::cout << std::endl;
  {
    /// test DummyQueue
    std::cout << "============ BEGIN TEST CONCURRENTQUEUE ============" << std::endl;
    ConcurrentQueue<int> q;
    std::thread t1([&q]() {
      auto v = *q.wait_and_pop();
      std::cout << "thread 1: " << v << std::endl;
    });
    std::thread t2([&q]() {
      auto v = *q.wait_and_pop();
      std::cout << "thread 2: " << v << std::endl;
    });
    std::thread t3([&q]() {
      auto v = *q.wait_and_pop();
      std::cout << "thread 3: " << v << std::endl;
    });

    q.push(1);
    q.push(2);
    q.push(3);

    t1.join();
    t2.join();
    t3.join();
    std::cout << "============= END TEST CONCURRENTQUEUE =============" << std::endl;
  }
  return 0;
}

