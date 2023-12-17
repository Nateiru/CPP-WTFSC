#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#define MEMORYPOOL_DEFAULT_BLOCK_SIZE 1024 * 1024

#include <cstddef> // usize_t
#include <cstdlib> // std::free
#include <utility> // std::forward

namespace Greptime {
namespace Memory {

// Simple error collection for memory pool
enum class EMemoryErrors {
  CANNOT_CREATE_MEMORY_POOL,
  CANNOT_CREATE_BLOCK,
  OUT_OF_POOL,
  EXCEEDS_MAX_SIZE,
  CANNOT_CREATE_BLOCK_CHAIN
};

// Header for a single memory block
struct SMemoryBlockHeader {
  // Block data
  size_t blockSize;
  size_t offset;

  // Movement to other blocks
  SMemoryBlockHeader* next;
  SMemoryBlockHeader* prev;

  // Garbage management data
  size_t numberOfAllocated;
  size_t numberOfDeleted;
};

// Header of a memory unit in the pool holding important metadata
struct SMemoryUnitHeader {
  size_t length;
  SMemoryBlockHeader* container;
};

template<typename T>
class MemoryPool {
public:
  /* Member types */
  typedef T               value_type;
  typedef T*              pointer;
  typedef T&              reference;
  typedef const T*        const_pointer;
  typedef const T&        const_reference;
  typedef size_t          size_type;
  typedef ptrdiff_t       difference_type;

  MemoryPool(size_t block_size = MEMORYPOOL_DEFAULT_BLOCK_SIZE);
  ~MemoryPool();

  template <typename U> struct rebind {
    typedef MemoryPool<U> other;
  };

  MemoryPool(const MemoryPool&) = delete;
  MemoryPool(MemoryPool&& other) noexcept;

  MemoryPool& operator=(const MemoryPool&) = delete;
  MemoryPool& operator=(MemoryPool&& other) noexcept;

  T* allocate(size_t n, const void* hint = 0);

  void deallocate(T* p, size_t n = 1);

  template <class U, class... Args>
  void construct(U* p, Args&&... args);

  template <class U>
  void destroy(U* p);

private:
  void createMemoryBlock(size_t block_size = MEMORYPOOL_DEFAULT_BLOCK_SIZE);

  void* allocatesize(size_t size);

private:
  SMemoryBlockHeader* firstBlock;
  SMemoryBlockHeader* currentBlock;
  size_t defaultBlockSize;
}; // MemoryPool

template<typename T>
MemoryPool<T>::MemoryPool(size_t block_size) {
  // Add first block to memory pool
  this->firstBlock = this->currentBlock = nullptr;
  this->defaultBlockSize = block_size;
  this->createMemoryBlock(block_size);
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
  SMemoryBlockHeader* block_iterator = firstBlock;
  while (block_iterator != nullptr) {
    SMemoryBlockHeader* next_iterator = block_iterator->next;
    std::free(block_iterator);
    block_iterator = next_iterator;
  }
}

template<typename T>
MemoryPool<T>::MemoryPool(MemoryPool<T> &&other) noexcept {
  firstBlock = other.firstBlock;
  currentBlock = other.currentBlock;
  defaultBlockSize = other.defaultBlockSize;
  // delete other
  other.firstBlock = other.currentBlock = nullptr;
  other.defaultBlockSize = MEMORYPOOL_DEFAULT_BLOCK_SIZE;
}

template<typename T>
MemoryPool<T>&
MemoryPool<T>::operator=(MemoryPool<T> &&other) noexcept {
  if (this == &other) 
    return *this;
  std::swap(firstBlock, other.firstBlock);
  std::swap(currentBlock, other.currentBlock);
  std::swap(defaultBlockSize, other.defaultBlockSize);
}

template<typename T>
void MemoryPool<T>::createMemoryBlock(size_t block_size)
{
	// Create the block
	SMemoryBlockHeader* block = reinterpret_cast<SMemoryBlockHeader*>(std::malloc(sizeof(SMemoryBlockHeader) + block_size));
	if (block == NULL) throw EMemoryErrors::CANNOT_CREATE_BLOCK;

	// Initalize block data
	block->blockSize = block_size;
	block->offset = 0;
	block->numberOfAllocated = 0;
	block->numberOfDeleted = 0;

	if (this->firstBlock != nullptr) {
		block->next = nullptr;
		block->prev = this->currentBlock;
		this->currentBlock->next = block;
		this->currentBlock = block;
	}
	else {
		block->next = block->prev = nullptr;
		this->firstBlock = block;
		this->currentBlock = block;
	}
}

template<typename T>
inline T* MemoryPool<T>::allocate(size_t n, const void* hint) {
  return reinterpret_cast<T*>(this->allocatesize(n * sizeof(T)));
}

template<typename T>
void* MemoryPool<T>::allocatesize(size_t size)
{
	// If there is enough space in current block then use the current block
	if (size + sizeof(SMemoryUnitHeader) < this->currentBlock->blockSize - this->currentBlock->offset);
	// Create new block if not enough space
	else if (size + sizeof(SMemoryUnitHeader) >= this->defaultBlockSize) this->createMemoryBlock(size + sizeof(SMemoryUnitHeader));
	else this->createMemoryBlock(this->defaultBlockSize);

	// Add unit
	SMemoryUnitHeader* unit = reinterpret_cast<SMemoryUnitHeader*>(reinterpret_cast<char*>(this->currentBlock) + sizeof(SMemoryBlockHeader) + this->currentBlock->offset);
	unit->length = size;
	unit->container = this->currentBlock;
	this->currentBlock->numberOfAllocated++;
	this->currentBlock->offset += sizeof(SMemoryUnitHeader) + size;

	return reinterpret_cast<char*>(unit) + sizeof(SMemoryUnitHeader);
}

template<typename T>
void MemoryPool<T>::deallocate(T* p, size_t n)
{
	if (p == nullptr) return;

	// Find unit
	SMemoryUnitHeader* unit = reinterpret_cast<SMemoryUnitHeader*>(reinterpret_cast<char*>(p) - sizeof(SMemoryUnitHeader));
	SMemoryBlockHeader* block = unit->container;

	// If last in block, then reset offset
	if (reinterpret_cast<char*>(block) + sizeof(SMemoryBlockHeader) + block->offset == reinterpret_cast<char*>(unit) + sizeof(SMemoryUnitHeader) + unit->length) {
		block->offset -= sizeof(SMemoryUnitHeader) + unit->length;
		block->numberOfAllocated--;
	}
	else block->numberOfDeleted++;

	// If block offset is 0 remove block if not the only one left
	if (this->currentBlock != this->firstBlock && (block->offset == 0 || block->numberOfAllocated == block->numberOfDeleted)) {
		if (block == this->firstBlock) {
			this->firstBlock = block->next;
			this->firstBlock->prev = nullptr;
		}
		else if (block == this->currentBlock) {
			this->currentBlock = block->prev;
			this->currentBlock->next = nullptr;
		}
		else {
			block->prev->next = block->next;
			block->next->prev = block->prev;
		}
		std::free(block);
	}
}

template <typename T>
template <class U, class... Args>
inline void
MemoryPool<T>::construct(U* p, Args&&... args)
{
  new (p) U (std::forward<Args>(args)...);
}

template <typename T>
template <class U>
inline void
MemoryPool<T>::destroy(U* p)
{
  p->~U();
}

}  // namespace Memory
}  // namespace Greptime

#endif // MEMORY_POOL_H 