#include <cstring>   // memset
#include <iostream>  // std::cout
#include <exception> // std::runtime_error

#include <sys/stat.h>// struct stat stat_buf
#include "disk_manager.h"

namespace Tiny {

static char *buffer_used;
#define LOG_DEBUG(X) std::cout << X << std::endl; 

DiskManager::DiskManager(const std::string &file_name) : file_name_(file_name) {

  std::scoped_lock scoped_io_latch_(io_latch_);
  io_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);

  if (!io_.is_open()) {
    // Set error state flags, which default is std::ios::goodbit
    io_.clear();
    io_.open(file_name_, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

    if (!io_.is_open()) {
      throw std::runtime_error("can't open file");
    }
  }
  buffer_used = nullptr;
}

void DiskManager::ShutDown() {
  std::scoped_lock scoped_db_io_latch(io_latch_);
  io_.close();
}

void DiskManager::WriteBlock(block_id_t block_id, const char *block_data) {
  std::scoped_lock scoped_io_latch(io_latch_);
  size_t offset = static_cast<size_t>(block_id) * BLOCK_SIZE;
  // set write cursor to offset
  num_writes_ += 1;
  io_.seekp(offset);
  io_.write(block_data, BLOCK_SIZE);
  // check for I/O error
  if (io_.bad()) {
    std::cout << "I/O error while writing" << std::endl;
    return;
  }
  // needs to flush to keep disk file in sync
  io_.flush();
}

void DiskManager::ReadBlock(block_id_t block_id, char *block_data) {
  std::scoped_lock scoped_io_latch(io_latch_);
  int offset = block_id * BLOCK_SIZE;
  // check if read beyond file length
  if (offset > GetFileSize(file_name_)) {
    LOG_DEBUG("I/O error reading past end of file");
  } else {
    // set read cursor to offset
    io_.seekp(offset);
    io_.read(block_data, BLOCK_SIZE);
    if (io_.bad()) {
      LOG_DEBUG("I/O error while reading");
      return;
    }
    // if file ends before reading BUSTUB_BLOCK_SIZE
    int read_count = io_.gcount();
    if (read_count < BLOCK_SIZE) {
      LOG_DEBUG("Read less than a page");
      io_.clear();
      memset(block_data + read_count, 0, BLOCK_SIZE - read_count);
    }
  }
}

auto DiskManager::GetNumWrites() const -> int { return num_writes_; }

auto DiskManager::GetFileSize(const std::string &file_name) -> int {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? static_cast<int>(stat_buf.st_size) : -1;
}

}  // namespace tony