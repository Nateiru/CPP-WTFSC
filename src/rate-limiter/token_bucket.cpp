// https://juejin.cn/s/%E4%BB%A4%E7%89%8C%E6%A1%B6%E7%AE%97%E6%B3%95%E5%AE%9E%E7%8E%B0%20c%2B%2B
#include <iostream>
#include <chrono>
#include <thread>

class TokenBucket {
public:
  TokenBucket(int rate, int capacity)
    : rate_(rate), capacity_(capacity), tokens_(0), last_time_(std::chrono::system_clock::now()) {}

  bool get_token() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elasped_seconds = now - last_time_;
    last_time_ = now;

    tokens_ += elasped_seconds.count() * rate_;
    if (tokens_ > capacity_) {
      tokens_ = capacity_;
    }

    if (tokens_ < 1.0) {
      return false;
    }
    else {
      tokens_ -= 1.0;
      return true;
    }
  }

private:
  double rate_;
  double capacity_;
  double tokens_;
  std::chrono::time_point<std::chrono::system_clock> last_time_;
};

int main() {
  TokenBucket tb(2, 10);  // 每秒生成2个令牌，容量为10个令牌
  std::this_thread::sleep_for(std::chrono::seconds(1));
  while (true) {
    if (tb.get_token()) {
      std::cout << "Got token" << std::endl;
    } else {
      std::cout << "not enough tokens" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
