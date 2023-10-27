#ifndef __SPARSEMATRIX_H__
#define	__SPARSEMATRIX_H__

#include <vector>
#include <iostream>
#include <stdexcept>

namespace SparseMatrix {

template<typename T>
class SparseMatrix {
public:

  SparseMatrix(int n); // square matrix n×n
  SparseMatrix(int rows, int columns); // general matrix
  ~SparseMatrix();

  int getRowCount(void) const;
  int getColumnCount(void) const;

  T get(int row, int col) const;
  SparseMatrix & set(int row, int col, T val);

  std::vector<T> multiply(const std::vector<T> & x) const;
  std::vector<T> operator * (const std::vector<T> & x) const;
  // 第 i 行和 vector<T> 相乘
  T multiplyRow(int i, const std::vector<T> & x) const;

  template<typename X>
  friend std::ostream & operator << (std::ostream & os, const SparseMatrix<X> & matrix);

private:
  int m, n;
  std::vector<T> * vals;
  std::vector<int> * rows, * cols;

  void insert(int index, int row, int col, T val);
  void remove(int index, int row);
};


template<typename T>
SparseMatrix<T>::SparseMatrix(int n) : SparseMatrix(n, n) {}

template<typename T>
SparseMatrix<T>::SparseMatrix(int rows, int columns) {

  if (rows < 1 || columns < 1) {
    throw std::runtime_error("Matrix dimensions cannot be zero or negative.");
  }

  this->m = rows;
  this->n = columns;

  this->vals = NULL;
  this->cols = NULL;
  this->rows = new std::vector<int>(rows + 1, 0);
}

template<typename T>
SparseMatrix<T>::~SparseMatrix() {
  if (this->vals != NULL) {
    delete this->vals;
    delete this->cols;
  }
  delete this->rows;
}

template<typename T>
int SparseMatrix<T>::getRowCount(void) const { return this->m; }

template<typename T>
int SparseMatrix<T>::getColumnCount(void) const { return this->n; }

template<typename T>
T SparseMatrix<T>::get(int row, int col) const {
  int cur_col;
  int row_start = (*(this->rows))[row];
  int row_end = (*(this->rows))[row + 1];
  for (int pos = row_start; pos < row_end; ++pos) {
    cur_col = (*(this->cols))[pos];

    if (cur_col == col) {
      return (*(this->vals))[pos];

    } else if (cur_col > col) {
      break;
    }
  }
  return T();
}

template<typename T>
SparseMatrix<T> & SparseMatrix<T>::set(int row, int col, T val) {

  int pos = (*(this->rows))[row];
  int cur_col = -1;

  for (; pos < (*(this->rows))[row + 1]; ++pos) {
    cur_col = (*(this->cols))[pos];

    if (cur_col >= col) {
      break;
    }
  }

  // 不存在则插入
  if (cur_col != col) {
    if (!(val == T())) {
      this->insert(pos, row, col, val);
    }

  } else if (val == T()) {
    // val 如果是无效值相当于删除
    this->remove(pos, row);

  } else {
    (*(this->vals))[pos] = val;
  }
  return *this;
}

template<typename T>
void SparseMatrix<T>::insert(int index, int row, int col, T val) {
  if (this->vals == NULL) {
    this->vals = new std::vector<T>(1, val);
    this->cols = new std::vector<int>(1, col);

  } else {
    this->vals->insert(this->vals->begin() + index, val);
    this->cols->insert(this->cols->begin() + index, col);
  }

  for (int i = row + 1; i <= this->m; i++) {
    (*(this->rows))[i] += 1;
  }
}

template<typename T>
void SparseMatrix<T>::remove(int index, int row) {
  this->vals->erase(this->vals->begin() + index);
  this->cols->erase(this->cols->begin() + index);

  for (int i = row + 1; i <= this->m; i++) {
    (*(this->rows))[i] -= 1;
  }
}

template<typename T>
std::vector<T> SparseMatrix<T>::operator * (const std::vector<T> & x) const {
  return this->multiply(x);
}

template<typename T>
std::vector<T> SparseMatrix<T>::multiply(const std::vector<T> & x) const {
  if (this->n != (int) x.size()) {
    throw std::runtime_error("Cannot multiply: Matrix column count and vector size don't match.");
  }

  std::vector<T> result(this->m, T());

  if (this->vals != NULL) { // only if any value set
    for (int i = 0; i < this->m; i++) {
      T sum = T();
      int row_start = (*(this->rows))[i];
      int row_end = (*(this->rows))[i + 1];
      for (int j = row_start; j < row_end; j++) {
        int col_idx = (*(this->cols))[j];
        T v = (*(this->vals))[j];
        sum = sum + v * x[col_idx];
      }
      result[i] = sum;
    }
  }
  return result;
}

template<typename T>
T SparseMatrix<T>::multiplyRow(int i, const std::vector<T> & x) const {
  if (this->n != (int) x.size()) {
    throw std::runtime_error("Cannot multiply: Matrix column count and vector size don't match.");
  }

  if (this->vals != NULL) { // only if any value set
    T sum = T();
    int row_start = (*(this->rows))[i];
    int row_end = (*(this->rows))[i + 1];
    for (int j = row_start; j < row_end; j++) {
      int col_idx = (*(this->cols))[j];
      T v = (*(this->vals))[j];
      sum = sum + v * x[col_idx];
    }
    return sum;
  }
  return T();
}

template<typename T>
std::ostream & operator << (std::ostream & os, const SparseMatrix<T> & matrix) {
  for (int i = 0; i < matrix.m; i++) {
    for (int j = 0; j < matrix.n; j++) {
      if (j != 0) {
        os << " ";
      }

      os << matrix.get(i, j);
    }

    if (i < matrix.m) {
      os << std::endl;
    }
  }

  return os;
}
}  // namespace SparseMatrix

#endif
