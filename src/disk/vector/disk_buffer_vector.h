#ifndef DISK_BUFFER_VECTOR_H
#define DISK_BUFFER_VECTOR_H

#include <cstddef>
#include <string>
#include "buffer_pool.h"

namespace Tiny {
template<typename T>
class DiskBufferVector {
public:
  DiskBufferVector() = default;
  ~DiskBufferVector() = default;
  explicit DiskBufferVector(BufferPool *buffer_pool);
  void put(size_t pos, T value);
  T get(size_t pos);

protected:
  static constexpr size_t BLOCK_SIZE = 4096;

private:
  BufferPool *buffer_pool_{nullptr};
};

template <typename T>
DiskBufferVector<T>::DiskBufferVector(BufferPool *buffer_pool) : buffer_pool_(buffer_pool) {

}

template <typename T>
T DiskBufferVector<T>::get(size_t pos) {
  Page *page = buffer_pool_->FetchPage(pos * sizeof(T) / PAGE_SIZE);
  char *data = page->GetData();
  size_t offset = pos * sizeof(T) % PAGE_SIZE;
  return *reinterpret_cast<T *>(data + offset);
}

template <typename T>
void DiskBufferVector<T>::put(size_t pos, T value) {
  Page *page = buffer_pool_->FetchPage(pos * sizeof(T) / PAGE_SIZE);
  char *data = page->GetData();
  size_t offset = pos * sizeof(T) % PAGE_SIZE;
  *reinterpret_cast<T *>(data + offset) = value;
}


}  // namespace Tiny
#endif
