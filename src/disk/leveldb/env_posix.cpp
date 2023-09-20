#include <unistd.h> // write

#include "slice.h"
#include "status.h"
#include "env_posix.h"

namespace Tiny {

Status PosixError(int error_number) {
  if (error_number == ENOENT) {
    return Status::NotFound("", std::strerror(error_number));
  } else {
    return Status::IOError("", std::strerror(error_number));
  }
}

Status PosixWritableFile::Append(const Slice& data) {
  size_t write_size = data.size();
  const char* write_data = data.data();

  // Fit as much as possible into buffer.
  size_t copy_size = std::min(write_size, BUFFER_SIZE - pos_);
  std::memcpy(buf_ + pos_, write_data, copy_size);
  write_data += copy_size;
  write_size -= copy_size;
  pos_ += copy_size;
  if (write_size == 0) {
    return Status::OK();
  }

  // Can't fit in buffer, so need to do at least one write.
  Status status = FlushBuffer();
  if (!status.ok()) {
    return status;
  }

  // Small writes go to buffer, large writes are written directly.
  if (write_size < BUFFER_SIZE) {
    std::memcpy(buf_, write_data, write_size);
    pos_ = write_size;
    return Status::OK();
  }
  return WriteUnbuffered(write_data, write_size);
}

Status PosixWritableFile::Close() {
  Status status = FlushBuffer();
  const int close_result = ::close(fd_);
  if (close_result < 0 && status.ok()) {
    status = PosixError(errno);
  }
  fd_ = -1;
  return status;
}

Status PosixWritableFile::Flush() { return FlushBuffer(); }

Status PosixWritableFile::Sync() {
  Status status = FlushBuffer();
  if (!status.ok()) {
    return status;
  }
  return SyncFd(fd_);
}

Status PosixWritableFile::FlushBuffer() {
  Status status = WriteUnbuffered(buf_, pos_);
  pos_ = 0;
  return status;
}

Status PosixWritableFile::WriteUnbuffered(const char* data, size_t size) {
  // 可能因为中断 write 写入的数据 < size
  // 此时需要继续写入
  while (size > 0) {
    ssize_t write_result = ::write(fd_, data, size);
    if (write_result < 0) {
      if (errno == EINTR) {
        continue;  // Retry
      }
      return PosixError(errno);
    }
    data += write_result;
    size -= write_result;
  }
  return Status::OK();
}

Status PosixWritableFile::SyncFd(int fd) {
#if HAVE_FULLFSYNC
  // On macOS and iOS, fsync() doesn't guarantee durability past power
  // failures. fcntl(F_FULLFSYNC) is required for that purpose. Some
  // filesystems don't support fcntl(F_FULLFSYNC), and require a fallback to
  // fsync().
  if (::fcntl(fd, F_FULLFSYNC) == 0) {
    return Status::OK();
  }
#endif  // HAVE_FULLFSYNC

#if HAVE_FDATASYNC
  bool sync_success = ::fdatasync(fd) == 0;
#else
  bool sync_success = ::fsync(fd) == 0;
#endif  // HAVE_FDATASYNC

  if (sync_success) {
    return Status::OK();
  }
  return PosixError(errno);
}

}  // namespace Tiny
