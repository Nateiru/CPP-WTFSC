#ifndef DISK_VECTOR_H
#define DISK_VECTOR_H

#include <fstream>    // std::fstream
#include <cstddef>    // size_t
#include <cstring>    // memset
#include <exception>  // std::runtime_error
#include <sys/stat.h> // struct stat stat_buf

namespace Tiny {
static char buffer[4096];
using block_id_t = size_t;

template<typename T>
class DiskVector {
public:
  DiskVector() = default;
  ~DiskVector();
  explicit DiskVector(const std::string &file_name);
  size_t size() const;
  void put(size_t pos, T value);
  T get(size_t pos);

protected:
  static constexpr size_t BLOCK_SIZE = 4096;

private:
  void ReadBlock(block_id_t block_id, char *block_data);
  void WriteBlock(block_id_t block_id, const char *block_data);
  std::fstream io_;
  std::string file_name_;
};

template<typename T>
DiskVector<T>::DiskVector(const std::string &file_name) : file_name_(file_name) {

  io_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);

  if (!io_.is_open()) {
    // Set error state flags, which default is std::ios::goodbit
    io_.clear();
    io_.open(file_name_, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

    if (!io_.is_open()) {
      throw std::runtime_error("can't open file");
    }
  }
}

template<typename T>
DiskVector<T>::~DiskVector() {
  io_.flush();
  io_.sync();
  io_.close();
}

template<typename T>
void DiskVector<T>::ReadBlock(block_id_t block_id, char *block_data) {
  int offset = block_id * BLOCK_SIZE;
  
  // set read cursor to offset
  io_.seekp(offset);
  io_.read(block_data, BLOCK_SIZE);
  if (io_.bad()) {
    return;
  }
  // if file ends before reading BUSTUB_BLOCK_SIZE
  int read_count = io_.gcount();
  if (read_count < BLOCK_SIZE) {
    io_.clear();
    memset(block_data + read_count, 0, BLOCK_SIZE - read_count);
  }
}

template<typename T>
void DiskVector<T>::WriteBlock(block_id_t block_id, const char *block_data) {
  size_t offset = static_cast<size_t>(block_id) * BLOCK_SIZE;
  // set write cursor to offset
  io_.seekp(offset);
  io_.write(block_data, BLOCK_SIZE);
  // check for I/O error
  if (io_.bad()) {
    return;
  }
  // needs to flush to keep disk file in sync
  io_.flush();
}

template <typename T>
T DiskVector<T>::get(size_t pos) {
  io_.seekp(pos * sizeof(T)); 
  T ret;
  io_.read(reinterpret_cast<char *>(&ret), sizeof(T));
  return ret;
}

template <typename T>
void DiskVector<T>::put(size_t pos, T value) {
  io_.seekp(pos * sizeof(T));
  io_.write(reinterpret_cast<char *>(&value), sizeof(T));
  io_.flush();
}

template<typename T>
size_t DiskVector<T>::size() const {
  struct stat stat_buf;
  int rc = stat(file_name_.c_str(), &stat_buf);
  return rc == 0 ? (static_cast<int>(stat_buf.st_size) / sizeof(T)) : -1;
}

}  // namespace Tiny

#endif
