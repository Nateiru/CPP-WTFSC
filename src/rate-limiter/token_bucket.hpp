// https://github.com/titan-web/rate-limit/blob/master/token_bucket/__init__.py
#include <chrono>

class TokenBucket {
public:
  TokenBucket(double capacity, double rate)
    : capacity_(capacity),
      tokens_(capacity),
      rate_(rate),
      last_time_(std::chrono::system_clock::now()) {}

  double get_tokens() {
    if (tokens_ < capacity_) {
      auto now = std::chrono::system_clock::now();
      std::chrono::duration<double> delta = rate_ * (now - last_time_);
      tokens_ = std::min(capacity_, tokens_ + delta.count());
      last_time_ = now;
    }
    return tokens_;
  }

  bool consume(double tokens) {
    if (tokens <= get_tokens()) {
      tokens_ -= tokens;
      return true;
    }
    return false;
  }

private:
  double capacity_;
  double tokens_;
  double rate_;
  std::chrono::time_point<std::chrono::system_clock> last_time_;
};
