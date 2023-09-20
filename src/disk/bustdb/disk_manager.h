#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <mutex>    // std::mutex
#include <string>   // std::string
#include <fstream>  // std::fstream
#include <iostream> // std::cout

namespace Tiny {

using block_id_t = int32_t;     // page id type
static constexpr int BLOCK_SIZE = 4096;
#define LOG_DEBUG(X) std::cout << X << std::endl; 

class DiskManager {
public:

  DiskManager() = default;
  explicit DiskManager(const std::string &file_name);
  virtual ~DiskManager() = default;

  // Shut down the disk manager and close all the file resources.
  void ShutDown();

  virtual void WriteBlock(block_id_t block_id, const char *block_data);
  virtual void ReadBlock(block_id_t block_id, char *block_data);

  auto GetNumWrites() const -> int;

protected:
  auto GetFileSize(const std::string &file_name) -> int;
  // stream to write file
  std::string file_name_;
  std::fstream io_;
  int num_writes_{0};

  // thread-safe to protect file access
  std::mutex io_latch_;
};

}  // namespace tony

#endif
