#include <cassert>
#include <chrono>   // time
#include <memory>   // std::unique_ptr
#include <iostream> // std::cout
#include <string>   // std::string


#include "MemoryPool.h"
#include "Vector.h"

/* Adjust these values depending on how much you trust your computer */
#define ELEMS 1000000
#define REPS 500

void TEST_VECTOR_MEMORY_POOL() {
  auto start_time = std::chrono::high_resolution_clock::now();
  Greptime::Container::Vector<int, Greptime::Memory::MemoryPool<int>> vector_pool;
  for (int j = 0; j < REPS; j++) {
    assert(vector_pool.empty());
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      vector_pool.push_back(i);
      vector_pool.push_back(i);
      vector_pool.push_back(i);
      vector_pool.push_back(i);
    }
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      vector_pool.pop_back();
      vector_pool.pop_back();
      vector_pool.pop_back();
      vector_pool.pop_back();
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "Int Test - MemoryPool Allocator Time: " << duration_ms.count() << " ms" << std::endl;
}

void TEST_VECTOR_STD() {
  auto start_time = std::chrono::high_resolution_clock::now();
  std::vector<int> stack_vector;
  for (int j = 0; j < REPS; j++)
  {
    assert(stack_vector.empty());
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      stack_vector.push_back(i);
      stack_vector.push_back(i);
      stack_vector.push_back(i);
      stack_vector.push_back(i);
    }
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      stack_vector.pop_back();
      stack_vector.pop_back();
      stack_vector.pop_back();
      stack_vector.pop_back();
    }
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "Int Test - Vector Std Time: " << duration_ms.count() << " ms" << std::endl;
}

void TEST_VECTOR_MEMORY_POOL_STRING() {
  auto start_time = std::chrono::high_resolution_clock::now();
  Greptime::Container::Vector<std::string, Greptime::Memory::MemoryPool<std::string>> vector_pool;
  for (int j = 0; j < REPS; j++) {
    assert(vector_pool.empty());
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      vector_pool.push_back("i");
      vector_pool.push_back("i");
      vector_pool.push_back("i");
      vector_pool.push_back("i");
    }
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      vector_pool.pop_back();
      vector_pool.pop_back();
      vector_pool.pop_back();
      vector_pool.pop_back();
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "String Test - MemoryPool Allocator Time: " << duration_ms.count() << " ms" << std::endl;
}

void TEST_VECTOR_STD_STRING() {
  auto start_time = std::chrono::high_resolution_clock::now();
  std::vector<std::string> stack_vector;
  for (int j = 0; j < REPS; j++)
  {
    assert(stack_vector.empty());
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      stack_vector.push_back("i");
      stack_vector.push_back("i");
      stack_vector.push_back("i");
      stack_vector.push_back("i");
    }
    for (int i = 0; i < ELEMS / 4; i++) {
      // Unroll to time the actual code and not the loop
      stack_vector.pop_back();
      stack_vector.pop_back();
      stack_vector.pop_back();
      stack_vector.pop_back();
    }
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "String Test - Vector Std Time: " << duration_ms.count() << " ms" << std::endl;
}

void TEST_USE() {
  Greptime::Container::Vector<int, Greptime::Memory::MemoryPool<int>> vector_pool;
  for (int j = 0; j < REPS; j++) {
    vector_pool.push_back(j);
    assert(vector_pool.size() == j + 1);
  }
  for (int j = 0; j < REPS; j++) {
    assert(vector_pool[j] == j);
  }

  vector_pool.clear();
  for (int j = 0; j < REPS; j++) {
    vector_pool.push_back(j);
    vector_pool.pop_back();
    assert(vector_pool.empty());
  }

  Greptime::Container::Vector<int, Greptime::Memory::MemoryPool<int>> vector_init{1, 2, 3, 4, 5, 6, 7};

  assert(vector_init.size() == 7);
  for (int j = 1; j <= 7; j++) assert(vector_init[j - 1] == j);

  Greptime::Container::Vector<int, Greptime::Memory::MemoryPool<int>> vec(10);
}

int main() {
  TEST_USE();
  // TEST_VECTOR_MEMORY_POOL();
  // TEST_VECTOR_STD();
  // TEST_VECTOR_MEMORY_POOL_STRING();
  // TEST_VECTOR_STD_STRING();
  return 0;
}