#include <cstddef>  // size_t
#include <cstdint>  // int32_t
#include <cstring>  // memset
#include <unordered_map>

#include "disk_manager.h"

namespace Tiny {

using page_id_t = int32_t;
using frame_id_t = int32_t;
constexpr size_t PAGE_SIZE = 4096;

class Page {
public:
  Page() { memset(data_, 0, PAGE_SIZE); }
  ~Page() = default;

  inline auto GetData() -> char * { return data_; }
  inline auto GetPageId() -> page_id_t { return page_id_; }
  inline auto IsDirty() -> bool { return is_dirty_; }

  page_id_t page_id_{-1};
  bool is_dirty_{false};

private:
  char data_[PAGE_SIZE]{};
};

class BufferPool {
public:
  BufferPool(size_t pool_size, DiskManager *disk_manager): pool_size_(pool_size), disk_manager_(disk_manager) {
    pages_ = new Page[pool_size_];
  }

  ~BufferPool() {
    delete [] pages_;
  }

  auto FetchPage(page_id_t page_id) -> Page * {
    frame_id_t frame_id = -1;
    if (page_table_.find(page_id) != page_table_.end()) {
      frame_id = page_table_[page_id];
      return &pages_[frame_id];
    }
    // 默认剔除 0
    frame_id = 0;
    for (frame_id_t i = 0; i < pool_size_; i++) {
      if (pages_[i].GetPageId() == -1) {
        frame_id = i;
        break;
      }
    }
    
    if (pages_[frame_id].IsDirty()) {
      // flush
      disk_manager_->WriteBlock(pages_[frame_id].GetPageId(), pages_[frame_id].GetData());
    }
    page_table_[page_id] = frame_id;

    disk_manager_->ReadBlock(page_id, pages_[frame_id].GetData());
    pages_[frame_id].page_id_ = page_id;
    pages_[frame_id].is_dirty_ = false;
    return &pages_[frame_id];
  }

private:
  const size_t pool_size_;
  Page *pages_;
  std::unordered_map<page_id_t, frame_id_t> page_table_;

  // disk manager
  DiskManager *disk_manager_;
};
}  // namespace Tiny

