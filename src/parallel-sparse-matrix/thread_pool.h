#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace ThreadPool {

class ThreadPool {
public:
  ThreadPool(size_t);
  template<typename F, typename...Args>
  auto enqueue(F &&f, Args &&... args)
  -> std::future<typename std::result_of<F(Args...)>::type>;
  ~ThreadPool();
private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  bool stop;

  // sync
  std::condition_variable cv;
  std::mutex mtx;
};

ThreadPool::ThreadPool(size_t n): stop(false) {
  for (size_t i = 0; i < n; ++i) {
    workers.emplace_back([this](){
      for (;;) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lk(this->mtx);
          cv.wait(lk, [this](){
            return this->stop || !this->tasks.empty();
          });

          if (this->stop && this->tasks.empty()) {
            break;
          }
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        task();
      }
    });
  }
}

template<typename F, typename...Args>
auto ThreadPool::enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
  );

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lk(mtx);
    if (stop) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    tasks.emplace([task]{
      (*task)();
    });
  }
  cv.notify_one();
  return res;
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lk(mtx);
    stop = true;
  }
  cv.notify_all();

  for(auto &worker : workers) {
    worker.join();
  }
}
} // namespace ThreadPool

#endif // #ifndef THREAD_POOL_H
