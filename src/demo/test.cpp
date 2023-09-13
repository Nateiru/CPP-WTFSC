#include <iostream>
#include <vector>

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

  std::vector<int> vec {1, 2, 3, 4, 5};
  std::string str("zhuziyi");

  Widget w1(vec, str);
  Widget w2(std::move(w1));
}