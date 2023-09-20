#include <vector>

class Matrix {
public:
  Matrix() : Matrix(0, 0) {}

  Matrix(std::size_t n_rows, std::size_t n_cols)
    : data(n_rows * n_cols), n_rows(n_rows), n_cols(n_cols) {
  }

  std::size_t row_count() const noexcept { return n_rows;}
  std::size_t col_count() const noexcept { return n_cols;}

  int &operator()(std::size_t i, std::size_t j) { return data[i * n_cols + j];}
  const int &operator()(std::size_t i, std::size_t j) const { return data[i * n_cols + j];}

private:
  std::vector<int> data;
  std::size_t n_rows;
  std::size_t n_cols;
};