#ifndef SLICE_H_
#define SLICE_H_

#include <cassert> // assert
#include <cstddef> // size_t
#include <cstring> // memcmp
#include <string>  // std::string

namespace Tiny {

/// Slice 只是数据的引用，并不持有数据。Slice 使用时要注意底层对象生命周期问题。
/// 目前没看到对内存数据的析构
class Slice {
 public:
  // Create an empty slice.
  Slice() : data_(""), size_(0) {}
  Slice(const char* d, size_t n) : data_(d), size_(n) {}
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}
  Slice(const char* s) : data_(s), size_(strlen(s)) {}

  Slice(const Slice&) = default;
  Slice& operator=(const Slice&) = default;

  const char* data() const { return data_; }
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  void clear() {
    data_ = "";
    size_ = 0;
  }

  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  std::string ToString() const { return std::string(data_, size_); }

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;

  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
  }

 private:
  // 不能修改指针指向的内容 但是能修改指针本身
  // 意味着 slice 指向的内容是不变的，改变它只能重新 malloc 
  const char* data_;
  size_t size_;
};

inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) { return !(x == y); }

inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_)
      r = -1;
    else if (size_ > b.size_)
      r = +1;
  }
  return r;
}

}  // namespace Tiny

#endif  // SLICE_H_
