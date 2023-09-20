#ifndef DISK_MANAGER_MEMORY_H
#define DISK_MANAGER_MEMORY_H

#include <mutex>         // shared_mutex
#include <array>         // std::array
#include <vector>        // std::vector
#include <memory>        // std::shared_ptr
#include <shared_mutex>  // std::shared_mutex

#include "disk_manager.h"
namespace Tiny {

class DiskManagerMemory : public DiskManager {
public:
  explicit DiskManagerMemory(size_t pages);
  ~DiskManagerMemory() override { delete[] memory_; }

  void WriteBlock(block_id_t block_id, const char *block_data) override;
  void ReadBlock(block_id_t block_id, char *block_data) override;

private:
  char *memory_;
};

class DiskManagerUnlimitedMemory : public DiskManager {
public:
  DiskManagerUnlimitedMemory() = default;
  ~DiskManagerUnlimitedMemory() override = default;

  void WriteBlock(block_id_t block_id, const char *block_data) override;
  void ReadBlock(block_id_t block_id, char *block_data) override;

private:
  std::mutex mutex_;
  using Block = std::array<char, BLOCK_SIZE>;
  using ProtectedBlock = std::pair<Block, std::shared_mutex>;
  std::vector<std::shared_ptr<ProtectedBlock>> data_;
};

}  // namespace Tiny
#endif
