// https://github.com/titan-web/rate-limit/blob/master/leaky_bucket/__init__.py
#include <chrono>
#include <memory>

class LeakyBucket {
public:
  LeakyBucket(double capacity, double leak_rate, bool is_lock = false)
    : capacity_(capacity),
      used_tokens_(0.0),
      rate_(leak_rate),
      last_time_(std::chrono::system_clock::now()) {}

  double get_tokens() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> delta = rate_ * (now - last_time_);
    used_tokens_ = std::max(0.0, used_tokens_ - delta.count());
    last_time_ = now;
    return used_tokens_;
  }

  bool consume(double tokens) {
    if (tokens + get_tokens() <= capacity_) {
      used_tokens_ += tokens;
      last_time_ = std::chrono::system_clock::now();
      return true;
    }
    return false;
  }

private:
  double capacity_;
  double used_tokens_;
  double rate_;
  std::chrono::time_point<std::chrono::system_clock> last_time_;
};
