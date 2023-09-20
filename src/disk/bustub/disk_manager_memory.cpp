#include <cstring>

#include "disk_manager_memory.h"

namespace Tiny {

DiskManagerMemory::DiskManagerMemory(size_t pages) { memory_ = new char[pages * BLOCK_SIZE]; }

void DiskManagerMemory::WriteBlock(block_id_t block_id, const char *block_data) {
  size_t offset = static_cast<size_t>(block_id) * BLOCK_SIZE;
  // set write cursor to offset
  num_writes_ += 1;
  memcpy(memory_ + offset, block_data, BLOCK_SIZE);
}

void DiskManagerMemory::ReadBlock(block_id_t block_id, char *block_data) {
  int64_t offset = static_cast<int64_t>(block_id) * BLOCK_SIZE;
  memcpy(block_data, memory_ + offset, BLOCK_SIZE);
}

void DiskManagerUnlimitedMemory::WriteBlock(block_id_t block_id, const char *block_data) {
  std::unique_lock<std::mutex> l(mutex_);
  if (block_id >= static_cast<int>(data_.size())) {
    data_.resize(block_id + 1);
  }
  if (data_[block_id] == nullptr) {
    data_[block_id] = std::make_shared<ProtectedBlock>();
  }
  std::shared_ptr<ProtectedBlock> ptr = data_[block_id];
  std::unique_lock<std::shared_mutex> l_page(ptr->second);
  l.unlock();

  memcpy(ptr->first.data(), block_data, BLOCK_SIZE);

}

void DiskManagerUnlimitedMemory::ReadBlock(block_id_t block_id, char *block_data) {
  std::unique_lock<std::mutex> l(mutex_);
  if (block_id >= static_cast<int>(data_.size()) || block_id < 0) {
    LOG_DEBUG("page not exist");
    return;
  }
  if (data_[block_id] == nullptr) {
    LOG_DEBUG("page not exist");
    return;
  }
  std::shared_ptr<ProtectedBlock> ptr = data_[block_id];
  std::shared_lock<std::shared_mutex> l_page(ptr->second);
  l.unlock();

  memcpy(block_data, ptr->first.data(), BLOCK_SIZE);
}

}  // namespace Tiny
