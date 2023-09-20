#ifndef ENV_POSIX_H
#define ENV_POSIX_H

#include <fcntl.h>
#include <cstddef>

#include "slice.h"
#include "status.h"

namespace Tiny {

// Common flags defined for all posix open operations
constexpr const int OpenFlags = O_CLOEXEC;
constexpr const size_t BUFFER_SIZE = 65536;

// A file abstraction for sequential writing.  The implementation
// must provide buffering since callers may append small fragments
// at a time to the file.
class WritableFile {
 public:
  WritableFile() = default;
  WritableFile(const WritableFile&) = delete;
  WritableFile& operator=(const WritableFile&) = delete;
  virtual ~WritableFile();

  virtual Status Append(const Slice& data) = 0;
  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;
};

class PosixWritableFile final : public WritableFile {
public:
  PosixWritableFile(int fd) : pos_(0), fd_(fd) {}
  ~PosixWritableFile() override {
    if (fd_ >= 0) {
      // Ignoring any potential errors
      Close();
    }
  }

  Status Append(const Slice& data) override;
  Status Close() override;
  Status Flush() override;
  Status Sync() override;

private:
  Status FlushBuffer();
  Status WriteUnbuffered(const char* data, size_t size);
  static Status SyncFd(int fd);

  // write buffer
  char buf_[BUFFER_SIZE];
  size_t pos_;

  // file descriptor
  int fd_;
};

}  // namespce Tiny
#endif
