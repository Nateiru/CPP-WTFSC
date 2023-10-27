#include <vector>
#include <chrono>
#include <cassert>
#include <iostream>

#include "thread_pool.h"
#include "sparse_matrix.h"

void TEST_THREAD_POOL() {
  ThreadPool::ThreadPool pool(4);
  std::vector<std::future<int>> results;

  for (int i = 0; i < 8; ++i) {
    results.emplace_back(
      pool.enqueue([i] {
        std::cout << "hello " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "world " << i << std::endl;
        return i * i;
      })
    );
  }
  for (auto &&result : results)
    std::cout << result.get() << ' ';
  std::cout << std::endl;
}

void TEST_SPARSE_MATRIX() {
  {
    SparseMatrix::SparseMatrix<int> a(2, 2);
    a.set(0, 0, 1); a.set(0, 1, 2);
                    a.set(1, 1, 1);
    std::cout << a << std::endl;
    std::vector<int> x{1, 1};
    std::vector<int> y = a * x;
    assert(y[0] == 3);
    assert(y[1] == 1);
  }

  {
    SparseMatrix::SparseMatrix<int> a(3, 3);
    a.set(0, 0, 1); a.set(0, 1, 1); a.set(0, 2, 1);
                    a.set(1, 1, 2); a.set(1, 2, 2);
                                    a.set(2, 2, 3);
    std::cout << a << std::endl;
    std::vector<int> x{1, 2, 3};
    std::vector<int> y = a * x;
    assert(y[0] == 6);
    assert(y[1] == 10);
    assert(y[2] == 9);
  }
}

void TEST_PARALLEL_SPARSE_MATRIX() {
  SparseMatrix::SparseMatrix<int> a(3, 3);
  a.set(0, 0, 1); a.set(0, 1, 1); a.set(0, 2, 1);
                  a.set(1, 1, 2); a.set(1, 2, 2);
                                  a.set(2, 2, 3);
  ThreadPool::ThreadPool pool(4);
  std::vector<int> x{1, 2, 3};
  std::vector<std::future<int>> y;

  for (int i = 0; i < 3; ++i) {
    y.emplace_back(
      pool.enqueue(&SparseMatrix::SparseMatrix<int>::multiplyRow, &a, i, x)
    );
  }

  assert(y[0].get() == 6);
  assert(y[1].get() == 10);
  assert(y[2].get() == 9);
}

int main() {
  TEST_THREAD_POOL();
  TEST_SPARSE_MATRIX();
  TEST_PARALLEL_SPARSE_MATRIX();
  return 0;
}
