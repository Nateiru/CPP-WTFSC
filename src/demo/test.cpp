#include <iostream>
#include <vector>
#include <memory>
#include <cstring>

class Widget {
public:
  Widget() = default;

  Widget(const std::vector<int> &vec, const std::string &str) : vec(vec), str(str) {}

  Widget(const Widget&) = default;

  Widget& operator = (const Widget&) = default;

  friend void Swap(Widget& a, Widget& b) {
    std::swap(a.vec, b.vec);
    std::swap(a.str, b.str);
  }

  Widget(Widget &&rhs) {
    Swap(*this, rhs);
  }

  Widget& operator = (Widget &&rhs) {
    Swap(*this, rhs);
    return *this;
  }

private:
  std::vector<int> vec;
  std::string str;
};

int main() {

  // int matrix [10000][10000];
  int** matrix = new int*[100000];
  for (int i = 0; i < 100000; i++) {
      matrix[i] = new int[100000];
  }


  // 记得在不再需要矩阵时释放内存
  for (int i = 0; i < 100000; i++) {
      delete[] matrix[i];
  }
  delete[] matrix;

  return 0;
}