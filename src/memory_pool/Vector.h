#ifndef VECTOR_ALLOC_H
#define VECTOR_ALLOC_H

#include <memory>           // allocator
#include <cstddef>          // size_t
#include <cassert>          // assert
#include <initializer_list> // std::initializer_list

#include "MemoryPool.h"

namespace Greptime {
namespace Container {

template <class T, class Alloc = std::allocator<T>>
class Vector {
public:
  Vector() {
    first_ = allocator_.allocate(1);
    last_ = first_;
    end_ = first_ + 1;
  }

  Vector(size_t size) {
    first_ = allocator_.allocate(size << 1);
    for (int i = 0; i < size; ++i) {
			allocator_.construct(first_ + i, T());
		}
    last_ = first_ + size;
    end_ = first_ + (size << 1);
  }

  ~Vector() {
 		for (T* p = first_; p != last_; ++p) {
			allocator_.destroy(p);
		}
    allocator_.deallocate(first_, 1);
    first_ = last_ = end_ = nullptr;
  }

  Vector(const Vector<T>& other) {
    size_t size = other.end_ - other.first_;
		//拷贝有效元素
		first_ = allocator_.allocate(size);
		size_t len = other.last_ - other.first_;
		for (int i = 0; i < len; ++i) {
			allocator_.construct(first_ + i, other.first_[i]);
		}

		last_ = first_ + len;
		end_ = first_ + size;
  }

  Vector(Vector<T>&& other) noexcept: 
              first_(other.first_), last_(other.last_),
              end_(other.end_), allocator_(std::move(other.allocator_)){
    other.first_ = other.last_ = other.end_ = nullptr;
  }

  Vector(std::initializer_list<T> init) {
    size_t size = init.size();
    int i = 0;
    first_ = allocator_.allocate(size << 1);
    for (const auto &v : init) {
      first_[i++] = v; 
    }
    last_ = first_ + size;
    end_ = first_ + (size << 1);
  }

  Vector<T>& operator=(const Vector<T> &other) {
    if (this == &other) {
      return *this;
    }

    // 先清空空间
    for (T* p = first_; p != last_; ++p) {
			allocator_.destroy(p);
		}
		allocator_.deallocate(first_, 1);

    // 复制
    size_t size = other.end_ - other.first_;
		// 拷贝有效元素
		first_ = allocator_.allocate(size);
		size_t len = other.last_ - other.first_;
		for (int i = 0; i < len; ++i) {
			allocator_.construct(first_ + i, other.first_[i]);
		}

		last_ = first_ + len;
		end_ = first_ + size;
    return *this;
  }

  Vector<T>& operator=(Vector<T> &&other) {
    if (this == &other) {
      return *this;
    }
    for (T* p = first_; p != last_; ++p) {
			allocator_.destroy(p);
		}
    first_ = other.first_;
    last_ = other.last_;
    end_ = other.end_;
    allocator_ = std::move(other.allocator_);
    other.first_ = other.last_ = other.end_ = nullptr;
    return *this;
  }

  void reserve(size_t n) {
    if (capacity() >= n) return;

    size_t len = size();
    T* ptmp = allocator_.allocate(n);
    for (int i = 0; i < len; ++i) {
      allocator_.construct(ptmp + i, first_[i]);
    }
    
    for (T* p = first_; p != last_; ++p) {
      allocator_.destroy(p);
    }
    allocator_.deallocate(first_, 1);

    first_ = ptmp;
    last_ = first_ + len;
    end_ = first_ + n;
  }

  void push_back(const T &val) {
		if (full())
			expand();
		allocator_.construct(last_, val);
		last_++;
	}

  void pop_back() {
		if (empty())
			return;
		--last_;
		allocator_.destroy(last_);
	}

  void clear() {
    for (T* p = first_; p != last_; ++p) {
			allocator_.destroy(p);
		}
    last_ = first_;
  }

  T back() const { return *(last_ - 1); }
  bool full() const { return last_ == end_; }
	bool empty() const { return first_ == last_; }
	size_t size() const { return last_ - first_; }
  size_t capacity() const { return end_ - first_; }
  T* data() { return first_; }
  const T* data() const { return first_; }

  T& at(size_t pos) { return first_[pos]; }
  const T& at(size_t pos) const { return first_[pos]; }

  const T& operator[](size_t pos) const { assert(pos < size()); return first_[pos];}
  T& operator[](size_t pos) { assert(pos < size()); return first_[pos];}

private:
  void expand() {
    size_t size = end_ - first_;
		T* ptmp = allocator_.allocate(size << 1);
		for (int i = 0; i < size; ++i) {
			allocator_.construct(ptmp + i, first_[i]);
		}
		
    for (T* p = first_; p != last_; ++p) {
			allocator_.destroy(p);
		}
		allocator_.deallocate(first_, 1);

		first_ = ptmp;
		last_ = first_ + size;
		end_ = first_ + (size << 1);
	}

private:
	T* first_;   //指向数组起始的位置
	T* last_;    //指向数组中有效元素的后继位置
	T* end_;     //指向数组空间的后继位置
  Alloc allocator_;
};

}  // namespace Memory
}  // namespace Greptime

#endif // VECTOR_ALLOC_H